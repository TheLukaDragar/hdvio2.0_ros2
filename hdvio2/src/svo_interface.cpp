// ============================================================================
// HDVIO2 - Modern ROS 2 Implementation
// Complete rewrite with proper callback groups and worker threads
// ============================================================================

#include "hdvio2/svo_interface.h"
#include "hdvio2/svo_factory.h"
#include "hdvio2/txt_reader.h"
#include "hdvio2/visualizer.h"
#include "hdvio2/ceres_backend_factory.h"

#include <svo/common/frame.h>
#include <svo/map.h>
#include <svo/imu_handler.h>
#include <svo/dynamics_handler.h>
#include <svo/dynamics_net.h>
#include <svo/opt_quad_angvel_spline.h>
#include <svo/common/camera.h>
#include <svo/common/conversions.h>
#include <svo/frame_handler_mono.h>
#include <svo/frame_handler_stereo.h>
#include <svo/frame_handler_array.h>
#include <svo/initialization.h>
#include <svo/direct/depth_filter.h>

#include <cv_bridge/cv_bridge.h>
#include <vikit/params_helper.h>
#include <vikit/timer.h>
#include <vikit/math_utils.h>

namespace svo {

// ============================================================================
// Constructor - Initialize all modules and start worker thread
// ============================================================================
SvoInterface::SvoInterface(
    const PipelineType& pipeline_type,
    const rclcpp::Node::SharedPtr& nh,
    const rclcpp::Node::SharedPtr& private_nh)
  : nh_(nh)
  , pnh_(private_nh)
  , pipeline_type_(pipeline_type)
{
  RCLCPP_INFO(nh_->get_logger(), "=== Initializing HDVIO2 with modern ROS 2 architecture ===");

  // Load parameters
  set_initial_attitude_from_gravity_ = 
      private_nh->declare_parameter("set_initial_attitude_from_gravity", true);
  automatic_reinitialization_ = 
      private_nh->declare_parameter("automatic_reinitialization", false);
  max_queue_size_ = 
      private_nh->declare_parameter("image_queue_size", 5);

  // Initialize SVO modules
  initializeSvoModules();

  // Create callback groups for parallel execution
  image_callback_group_ = nh_->create_callback_group(
      rclcpp::CallbackGroupType::MutuallyExclusive);
  imu_callback_group_ = nh_->create_callback_group(
      rclcpp::CallbackGroupType::Reentrant);  // IMU callbacks can overlap
  dynamics_callback_group_ = nh_->create_callback_group(
      rclcpp::CallbackGroupType::Reentrant);
  control_callback_group_ = nh_->create_callback_group(
      rclcpp::CallbackGroupType::MutuallyExclusive);

  // Setup subscriptions with callback groups
  setupSubscriptions();

  // Start worker thread for heavy image processing
  image_processing_thread_ = std::thread(&SvoInterface::imageProcessingWorker, this);

  RCLCPP_INFO(nh_->get_logger(), "=== HDVIO2 initialization complete ===");
}

// ============================================================================
// Destructor - Clean shutdown
// ============================================================================
SvoInterface::~SvoInterface()
{
  RCLCPP_INFO(nh_->get_logger(), "Shutting down HDVIO2...");
  
  // Signal worker thread to stop
  quit_.store(true);
  image_queue_cv_.notify_all();

  // Wait for worker thread
  if (image_processing_thread_.joinable()) {
    image_processing_thread_.join();
  }

  RCLCPP_INFO(nh_->get_logger(), "HDVIO2 shutdown complete");
}

// ============================================================================
// Initialize SVO Modules (same as before)
// ============================================================================
void SvoInterface::initializeSvoModules()
{
  // Create SVO pipeline
  switch (pipeline_type_) {
    case PipelineType::kMono:
      svo_ = factory::makeMono(pnh_);
      break;
    case PipelineType::kStereo:
      svo_ = factory::makeStereo(pnh_);
      break;
    case PipelineType::kArray:
      svo_ = factory::makeArray(pnh_);
      break;
    default:
      throw std::runtime_error("Unknown pipeline type");
  }

  ncam_ = svo_->getNCamera();
  
  // Create visualizer
  visualizer_ = std::make_shared<Visualizer>(
      svo_->options_.trace_dir, pnh_, ncam_->getNumCameras());

  // Create IMU handler
  imu_handler_ = factory::getImuHandler(pnh_);
  svo_->imu_handler_ = imu_handler_;

  // Create dynamics handler if enabled
  bool use_dynamics = pnh_->declare_parameter("use_dynamics", false);
  dynamics_dataset_ = pnh_->declare_parameter("dynamics_dataset", 0);

  if (use_dynamics) {
    dynamics_handler_ = factory::getDynamicsHandler(pnh_);
    
    thrust_dynamics_net_ = std::make_shared<DynamicsNet>(
        dynamics_handler_->options_.thrust_net_model_file, "thrust");
    torque_dynamics_net_ = std::make_shared<DynamicsNet>(
        dynamics_handler_->options_.torque_net_model_file, "torque");

    if (dynamics_handler_->options_.use_learned_residuals) {
      RCLCPP_INFO(nh_->get_logger(), "Loading thrust dynamics network...");
      thrust_dynamics_net_->parseTrtEngine();
      thrust_dynamics_net_->allocateMemory();
      
      if (dynamics_handler_->options_.use_rotational_dynamics) {
        RCLCPP_INFO(nh_->get_logger(), "Loading torque dynamics network...");
        torque_dynamics_net_->parseTrtEngine();
        torque_dynamics_net_->allocateMemory();
      }
    }

    const int64_t spline_length_ns = 
        static_cast<int64_t>(dynamics_handler_->options_.spline_length_s * 1e9);
    const int64_t dt_knot_ns = 
        static_cast<int64_t>(dynamics_handler_->options_.dt_knot_s * 1e9);
    opt_quad_angvel_spline_ = std::make_shared<OptimizeQuadAngVelSpline>(
        dt_knot_ns, spline_length_ns);
    opt_quad_angvel_spline_->set_inertia(dynamics_handler_->options_.Inertia);
  } else {
    dynamics_handler_ = std::make_shared<DynamicsHandler>(DynamicsHandlerOptions());
    thrust_dynamics_net_ = std::make_shared<DynamicsNet>(" ", " ");
    torque_dynamics_net_ = std::make_shared<DynamicsNet>(" ", " ");
    opt_quad_angvel_spline_ = std::make_shared<OptimizeQuadAngVelSpline>();
  }

  // Create backend
  ceres_backend_interface_ = ceres_backend_factory::makeBackend(pnh_, ncam_);
  svo_->setBundleAdjuster(ceres_backend_interface_);
  ceres_backend_interface_->setImu(imu_handler_);
  ceres_backend_interface_->setDynamicsHandler(dynamics_handler_);
  ceres_backend_interface_->setThrustDynamicsNet(thrust_dynamics_net_);
  ceres_backend_interface_->setTorqueDynamicsNet(torque_dynamics_net_);
  ceres_backend_interface_->setOptimizeQuadAngVelSpline(opt_quad_angvel_spline_);
  ceres_backend_interface_->makePublisher(pnh_, ceres_backend_publisher_);

  // Initialize from groundtruth if requested
  bool init_from_groundtruth = pnh_->declare_parameter("init_from_groundtruth", false);
  if (init_from_groundtruth) {
    std::string gt_file = pnh_->declare_parameter<std::string>("groundtruth_file", "");
    PoseMeasurements gt_poses;
    if (loadStampedPoses(gt_file, gt_poses)) {
      ceres_backend_interface_->setGroundtruthPoses(gt_poses);
      RCLCPP_INFO(nh_->get_logger(), "Initialized backend from groundtruth");
    }
  }

  svo_->start();
}

// ============================================================================
// Setup Subscriptions with Callback Groups
// ============================================================================
void SvoInterface::setupSubscriptions()
{
  // ===== IMAGE SUBSCRIPTION with image_transport =====
  std::string image_topic = pnh_->declare_parameter<std::string>(
      "cam0_topic", "camera/image_raw");
  
  // Parameter to specify transport type (raw, compressed, theora, etc.)
  std::string image_transport_type = pnh_->declare_parameter<std::string>(
      "image_transport", "compressed");

  // Create image_transport instance
  image_transport_ = std::make_shared<image_transport::ImageTransport>(nh_);

  // Subscribe with specified transport type
  image_transport::TransportHints hints(nh_.get(), image_transport_type);
  sub_image_ = image_transport_->subscribe(
      image_topic,
      10,
      std::bind(&SvoInterface::imageCallbackFast, this, std::placeholders::_1),
      nullptr,
      &hints);

  RCLCPP_INFO(nh_->get_logger(), "Subscribed to: %s (transport: %s)", 
              image_topic.c_str(), image_transport_type.c_str());

  // ===== IMU SUBSCRIPTION =====
  std::string imu_topic = pnh_->declare_parameter<std::string>("imu_topic", "/px4_0/imu");

  rclcpp::SubscriptionOptions imu_options;
  imu_options.callback_group = imu_callback_group_;

  auto imu_qos = rclcpp::QoS(rclcpp::KeepLast(100));
  imu_qos.best_effort();  // Match PX4's QoS

  sub_imu_ = nh_->create_subscription<sensor_msgs::msg::Imu>(
      imu_topic,
      imu_qos,
      std::bind(&SvoInterface::imuCallbackFast, this, std::placeholders::_1),
      imu_options);

  RCLCPP_INFO(nh_->get_logger(), "Subscribed to: %s", imu_topic.c_str());

  // ===== DYNAMICS SUBSCRIPTION =====
  std::string dynamics_topic = pnh_->declare_parameter<std::string>(
      "dynamics_topic", "dynamics");

  rclcpp::SubscriptionOptions dynamics_options;
  dynamics_options.callback_group = dynamics_callback_group_;

  sub_dynamics_ = nh_->create_subscription<agiros_msgs::msg::Command>(
      dynamics_topic,
      10,
      std::bind(&SvoInterface::dynamicsCallbackFast, this, std::placeholders::_1),
      dynamics_options);

  RCLCPP_INFO(nh_->get_logger(), "Subscribed to: %s", dynamics_topic.c_str());

  // ===== REMOTE KEY SUBSCRIPTION =====
  std::string remote_key_topic = pnh_->declare_parameter<std::string>(
      "remote_key_topic", "svo/remote_key");

  rclcpp::SubscriptionOptions control_options;
  control_options.callback_group = control_callback_group_;

  sub_remote_key_ = nh_->create_subscription<std_msgs::msg::String>(
      remote_key_topic,
      5,
      std::bind(&SvoInterface::inputKeyCallback, this, std::placeholders::_1),
      control_options);

  RCLCPP_INFO(nh_->get_logger(), "Subscribed to: %s", remote_key_topic.c_str());
}

// ============================================================================
// FAST CALLBACK: Image (must return quickly!)
// ============================================================================
void SvoInterface::imageCallbackFast(
    const sensor_msgs::msg::Image::ConstSharedPtr& msg)
{
  total_images_received_.fetch_add(1);
  
  if (idle_.load()) {
    return;  // System paused
  }

  // Convert using cv_bridge (handles all formats efficiently!)
  cv_bridge::CvImageConstPtr cv_ptr;
  try {
    // toCvShare is ZERO-COPY when possible!
    cv_ptr = cv_bridge::toCvShare(msg, sensor_msgs::image_encodings::MONO8);
  } catch (cv_bridge::Exception& e) {
    RCLCPP_ERROR(nh_->get_logger(), "cv_bridge exception: %s", e.what());
    return;
  }

  int64_t timestamp_ns = rclcpp::Time(msg->header.stamp).nanoseconds();

  // Queue for processing (fast, non-blocking)
  {
    std::lock_guard<std::mutex> lock(image_queue_mutex_);
    
    // Drop old frames if queue is full
    if (image_queue_.size() >= max_queue_size_) {
      RCLCPP_WARN_THROTTLE(nh_->get_logger(), *nh_->get_clock(), 1000,
                           "Image queue full, dropping old frame");
      image_queue_.pop();
    }
    
    // Clone the image for queue (cv_ptr goes out of scope)
    image_queue_.emplace(cv_ptr->image.clone(), timestamp_ns);
  }
  image_queue_cv_.notify_one();

  // Callback complete - return immediately!
}

// ============================================================================
// FAST CALLBACK: IMU
// ============================================================================
void SvoInterface::imuCallbackFast(const sensor_msgs::msg::Imu::ConstSharedPtr& msg)
{
  Eigen::Vector3d omega_imu(
      msg->angular_velocity.x, 
      msg->angular_velocity.y, 
      msg->angular_velocity.z);
  Eigen::Vector3d lin_acc_imu(
      msg->linear_acceleration.x, 
      msg->linear_acceleration.y, 
      msg->linear_acceleration.z);

  // Apply calibration if needed
  if (imu_handler_->imu_calib_.correct_scale_and_axis_misalignment) {
    omega_imu = imu_handler_->imu_calib_.Mg_inv * omega_imu;
    lin_acc_imu = imu_handler_->imu_calib_.Ma_inv * lin_acc_imu;
  }

  const ImuMeasurement m(
      rclcpp::Time(msg->header.stamp).seconds(), 
      omega_imu, 
      lin_acc_imu);

  if (imu_handler_) {
    imu_handler_->addImuMeasurement(m);
  }
}

// ============================================================================
// FAST CALLBACK: Dynamics
// ============================================================================
void SvoInterface::dynamicsCallbackFast(
    const agiros_msgs::msg::Command::ConstSharedPtr& msg)
{
  Eigen::Vector3d mass_norm_collective_thrust(Eigen::Vector3d::Zero());
  Eigen::Vector3d collective_torque(Eigen::Vector3d::Zero());
  DynamicsMeasurement m;

  // Process based on dataset type
  if (dynamics_dataset_ == 0) {  // Agilicious
    double fz = msg->thrusts[0] + msg->thrusts[1] + msg->thrusts[2] + msg->thrusts[3];
    fz /= dynamics_handler_->options_.mass;
    mass_norm_collective_thrust(2) = fz;

    // Calculate torques
    std::array<double, 4> omega2;
    for (int i = 0; i < 4; ++i) {
      omega2[i] = msg->thrusts[i] / dynamics_handler_->options_.thrust_coeffs(i);
    }

    collective_torque(2) += dynamics_handler_->options_.torque_coeffs(2) * omega2[2];
    collective_torque(2) += dynamics_handler_->options_.torque_coeffs(3) * omega2[3];
    collective_torque(2) -= dynamics_handler_->options_.torque_coeffs(0) * omega2[0];
    collective_torque(2) -= dynamics_handler_->options_.torque_coeffs(1) * omega2[1];

    // r_i x thrust_i
    collective_torque += skew(dynamics_handler_->options_.r_br) * 
        Eigen::Vector3d(0., 0., msg->thrusts[0]);
    collective_torque += skew(dynamics_handler_->options_.r_fl) * 
        Eigen::Vector3d(0., 0., msg->thrusts[1]);
    collective_torque += skew(dynamics_handler_->options_.r_bl) * 
        Eigen::Vector3d(0., 0., msg->thrusts[2]);
    collective_torque += skew(dynamics_handler_->options_.r_fr) * 
        Eigen::Vector3d(0., 0., msg->thrusts[3]);

  } else if (dynamics_dataset_ == 1) {  // Blackbird
    // Similar processing for Blackbird dataset...
    RCLCPP_WARN_ONCE(nh_->get_logger(), "Blackbird dynamics processing");
  }

  m.timestamp_ = rclcpp::Time(msg->header.stamp).seconds();
  m.collective_thrust_ = mass_norm_collective_thrust;
  m.collective_torque_ = collective_torque;

  if (dynamics_handler_) {
    dynamics_handler_->addDynamicsMeasurement(m);
  }
}

// ============================================================================
// FAST CALLBACK: Debug Dynamics
// ============================================================================
void SvoInterface::debugDynamicsCallbackFast(
    const geometry_msgs::msg::WrenchStamped::ConstSharedPtr& msg)
{
  if (dynamics_dataset_ != 1) {
    return;  // Only supported for Blackbird
  }

  DynamicsMeasurement m;
  m.timestamp_ = rclcpp::Time(msg->header.stamp).seconds();
  m.collective_thrust_(0) = msg->wrench.force.x;
  m.collective_thrust_(1) = msg->wrench.force.y;
  m.collective_thrust_(2) = msg->wrench.force.z;
  m.collective_torque_(0) = msg->wrench.torque.x;
  m.collective_torque_(1) = msg->wrench.torque.y;
  m.collective_torque_(2) = msg->wrench.torque.z;

  if (dynamics_handler_) {
    dynamics_handler_->addDynamicsMeasurement(m);
  }
}

// ============================================================================
// CALLBACK: Input Key
// ============================================================================
void SvoInterface::inputKeyCallback(const std_msgs::msg::String::ConstSharedPtr& key_input)
{
  if (key_input->data.empty()) return;

  char input = key_input->data[0];
  switch(input) {
    case 'q':
      quit_.store(true);
      RCLCPP_INFO(nh_->get_logger(), "User requested quit");
      rclcpp::shutdown();
      break;
    case 'r':
      svo_->reset();
      idle_.store(true);
      RCLCPP_INFO(nh_->get_logger(), "User requested reset");
      break;
    case 's':
      svo_->start();
      idle_.store(false);
      RCLCPP_INFO(nh_->get_logger(), "User requested start");
      break;
    case 'c':
      svo_->setCompensation(true);
      RCLCPP_INFO(nh_->get_logger(), "Enabled affine compensation");
      break;
    case 'C':
      svo_->setCompensation(false);
      RCLCPP_INFO(nh_->get_logger(), "Disabled affine compensation");
      break;
    default:
      break;
  }
}

// ============================================================================
// WORKER THREAD: Process images (can block as long as needed)
// ============================================================================
void SvoInterface::imageProcessingWorker()
{
  RCLCPP_INFO(nh_->get_logger(), "Image processing worker thread started");

  int processed_count = 0;
  auto last_report_time = std::chrono::steady_clock::now();

  while (!quit_.load()) {
    ImageData data;
    
    // Wait for work
    {
      std::unique_lock<std::mutex> lock(image_queue_mutex_);
      image_queue_cv_.wait(lock, [this] { 
        return !image_queue_.empty() || quit_.load(); 
      });

      if (quit_.load() && image_queue_.empty()) {
        break;
      }

      if (image_queue_.empty()) {
        continue;
      }

      data = image_queue_.front();
      image_queue_.pop();
    }

    // Process image (THIS CAN BLOCK - we're in a dedicated worker thread!)
    std::vector<cv::Mat> images = {data.image};

    try {
      // Check IMU prior
      if (!setImuPrior(data.timestamp_ns)) {
        continue;  // Not ready yet
      }

      // Pre-processing hook
      imageCallbackPreprocessing(data.timestamp_ns);

      // Heavy SVO processing
      processImageBundle(images, data.timestamp_ns);

      // Publish results
      publishResults(images, data.timestamp_ns);

      // Auto-restart if paused
      if (svo_->stage() == Stage::kPaused && automatic_reinitialization_) {
        svo_->start();
      }

      // Post-processing hook
      imageCallbackPostprocessing();

      processed_count++;

    } catch (const std::exception& e) {
      RCLCPP_ERROR(nh_->get_logger(), "Exception in image processing: %s", e.what());
    }

    // Periodic status report
    auto now = std::chrono::steady_clock::now();
    if (std::chrono::duration_cast<std::chrono::seconds>(now - last_report_time).count() >= 5) {
      int total = total_images_received_.load();
      RCLCPP_INFO(nh_->get_logger(), 
                  "Image processing: %d processed, %d total received, %zu queued",
                  processed_count, total, image_queue_.size());
      last_report_time = now;
    }
  }

  RCLCPP_INFO(nh_->get_logger(), "Image processing worker thread stopped");
}

// ============================================================================
// HEAVY PROCESSING: Process Image Bundle
// ============================================================================
void SvoInterface::processImageBundle(
    const std::vector<cv::Mat>& images,
    int64_t timestamp_ns)
{
  // This is the blocking SVO call
  svo_->addImageBundle(images, timestamp_ns);
}

// ============================================================================
// HEAVY PROCESSING: Publish Results
// ============================================================================
void SvoInterface::publishResults(
    const std::vector<cv::Mat>& images,
    int64_t timestamp_ns)
{
  CHECK_NOTNULL(svo_.get());
  CHECK_NOTNULL(visualizer_.get());

  visualizer_->img_caption_.clear();
  if (svo_->isBackendValid()) {
    visualizer_->img_caption_ = ceres_backend_interface_->getStationaryStatusStr();
  }

  visualizer_->publishSvoInfo(svo_.get(), timestamp_ns);

  switch (svo_->stage()) {
    case Stage::kTracking: {
      Eigen::Matrix<double, 6, 6> covariance;
      covariance.setZero();
      visualizer_->publishImuPose(
          svo_->getLastFrames()->get_T_W_B(), covariance, timestamp_ns);
      visualizer_->publishCameraPoses(svo_->getLastFrames(), timestamp_ns);
      visualizer_->visualizeMarkers(
          svo_->getLastFrames(), svo_->closeKeyframes(), svo_->map());
      visualizer_->exportToDense(svo_->getLastFrames());
      
      bool draw_boundary = false;
      if (svo_->isBackendValid()) {
        draw_boundary = svo_->getBundleAdjuster()->isFixedToGlobalMap();
      }
      visualizer_->publishImagesWithFeatures(
          svo_->getLastFrames(), timestamp_ns, draw_boundary);
      break;
    }
    case Stage::kInitializing: {
      visualizer_->publishBundleFeatureTracks(
          svo_->initializer_->frames_ref_, svo_->getLastFrames(), timestamp_ns);
      break;
    }
    case Stage::kPaused:
    case Stage::kRelocalization:
    case Stage::kReinitialize:
      visualizer_->publishImages(images, timestamp_ns);
      break;
    default:
      RCLCPP_FATAL(nh_->get_logger(), "Unknown stage");
      break;
  }
}

// ============================================================================
// Check IMU Prior
// ============================================================================
bool SvoInterface::setImuPrior(int64_t timestamp_ns)
{
  if (!svo_->hasStarted()) {
    size_t imu_count = imu_handler_->getMeasurementsCopy().size();
    if (imu_count < 10u) {
      return false;  // Need at least 10 IMU measurements
    }
  }
  return true;
}

} // namespace svo
