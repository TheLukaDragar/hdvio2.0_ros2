#include <hdvio2/svo_interface.h>

#include <rclcpp/rclcpp.hpp>

#include <hdvio2/svo_factory.h>
#include <hdvio2/txt_reader.h>
#include <hdvio2/visualizer.h>
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

#include <message_filters/subscriber.h>
#include <message_filters/synchronizer.h>
#include <message_filters/sync_policies/exact_time.h>
#include <message_filters/sync_policies/approximate_time.h>
#include <image_transport/subscriber_filter.hpp>
#include <image_transport/image_transport.hpp>
#include <sensor_msgs/image_encodings.hpp>
#include <cv_bridge/cv_bridge.h>
#include <vikit/params_helper.h>
#include <vikit/timer.h>
#include <vikit/math_utils.h>
#include <hdvio2/ceres_backend_factory.h>


namespace svo {

// Global counter for debugging image delivery
static std::atomic<int> g_total_images_received{0};

SvoInterface::SvoInterface(
    const PipelineType& pipeline_type,
    const rclcpp::Node::SharedPtr& nh,
    const rclcpp::Node::SharedPtr& private_nh)
  : nh_(nh)
  , pnh_(private_nh)
  , pipeline_type_(pipeline_type)
  , set_initial_attitude_from_gravity_(
      pnh_->declare_parameter("set_initial_attitude_from_gravity", true))
  , automatic_reinitialization_(
      pnh_->declare_parameter("automatic_reinitialization", false))
{
  switch (pipeline_type)
  {
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
      LOG(FATAL) << "Unknown pipeline";
      break;
  }
  ncam_ = svo_->getNCamera();

  visualizer_.reset(
        new Visualizer(svo_->options_.trace_dir, pnh_, ncam_->getNumCameras()));

  imu_handler_ = factory::getImuHandler(pnh_);
  svo_->imu_handler_ = imu_handler_;

  bool use_dynamics = pnh_->declare_parameter("use_dynamics", false);
  size_t dynamics_dataset = pnh_->declare_parameter("dynamics_dataset", 0);
  dynamics_dataset_ = dynamics_dataset;

  if (use_dynamics)
  {
    // initialize handler to manage dynamics measurements
    dynamics_handler_ = factory::getDynamicsHandler(pnh_);
    
    // initialize networks for dynamics residuals
    DynamicsNet::Ptr thrust_dynamics_net(new DynamicsNet(
      dynamics_handler_->options_.thrust_net_model_file, "thrust"));
    thrust_dynamics_net_ = thrust_dynamics_net;
    DynamicsNet::Ptr torque_dynamics_net(new DynamicsNet(
      dynamics_handler_->options_.torque_net_model_file, "torque"));
    torque_dynamics_net_ = torque_dynamics_net;
    if (dynamics_handler_->options_.use_learned_residuals)
    {
      SVO_INFO_STREAM("Using learned thrust residuals");
      SVO_INFO_STREAM("Loading thrust net ...");
      thrust_dynamics_net_->parseTrtEngine();
      thrust_dynamics_net_->allocateMemory();
      SVO_INFO_STREAM("Done setting up thrust net ...");

      if (dynamics_handler_->options_.use_rotational_dynamics)
      {
        SVO_INFO_STREAM("Using learned torque residuals");
        SVO_INFO_STREAM("Loading torque net ...");
        torque_dynamics_net_->parseTrtEngine();
        torque_dynamics_net_->allocateMemory();
        SVO_INFO_STREAM("Done setting up torque net ...");
      }
    }

    // initialize spline for rotational dynamics
    const int64_t spline_length_ns = static_cast<int64_t>(dynamics_handler_->options_.spline_length_s*1e9);
    const int64_t dt_knot_ns = static_cast<int64_t>(dynamics_handler_->options_.dt_knot_s*1e9);
    OptimizeQuadAngVelSpline::Ptr opt_quad_angvel_spline(new OptimizeQuadAngVelSpline(dt_knot_ns, spline_length_ns));
    opt_quad_angvel_spline_ = opt_quad_angvel_spline;
    opt_quad_angvel_spline_->set_inertia(dynamics_handler_->options_.Inertia);
  }
  else
  {
    DynamicsHandlerOptions options;
    DynamicsHandler::Ptr dynamics_handler(new DynamicsHandler(options));
    dynamics_handler_ = dynamics_handler;
    
    DynamicsNet::Ptr thrust_dynamics_net(new DynamicsNet(" ", " "));
    thrust_dynamics_net_ = thrust_dynamics_net;

    DynamicsNet::Ptr torque_dynamics_net(new DynamicsNet(" ", " "));
    torque_dynamics_net_ = torque_dynamics_net;

    OptimizeQuadAngVelSpline::Ptr opt_quad_angvel_spline(new OptimizeQuadAngVelSpline());
    opt_quad_angvel_spline_ = opt_quad_angvel_spline;
  }

  ceres_backend_interface_ = ceres_backend_factory::makeBackend(pnh_,ncam_);
  svo_->setBundleAdjuster(ceres_backend_interface_);
  ceres_backend_interface_->setImu(imu_handler_);
  ceres_backend_interface_->setDynamicsHandler(dynamics_handler_);
  ceres_backend_interface_->setThrustDynamicsNet(thrust_dynamics_net_);
  ceres_backend_interface_->setTorqueDynamicsNet(torque_dynamics_net_);
  ceres_backend_interface_->setOptimizeQuadAngVelSpline(opt_quad_angvel_spline_);
  ceres_backend_interface_->makePublisher(pnh_, ceres_backend_publisher_);

  bool init_from_groundtruth = pnh_->declare_parameter("init_from_groundtruth", false);
  if (init_from_groundtruth)
  {
    SVO_INFO_STREAM("Initizalizing backend from groundtruth pose!");
    std::string gt_file = pnh_->declare_parameter<std::string>("groundtruth_file", "");

    PoseMeasurements gt_poses;
    bool loaded_gt = loadStampedPoses(gt_file, gt_poses);

    if (loaded_gt)
    {
      ceres_backend_interface_->setGroundtruthPoses(gt_poses);
    }
    else
    {
      SVO_INFO_STREAM("Initizalization from groundtruth unsuccessful! .txt was not found!");
    }
  }

  svo_->start();
}

SvoInterface::~SvoInterface()
{
  if (imu_thread_)
    imu_thread_->join();
  if (image_thread_)
    image_thread_->join();
  VLOG(1) << "Destructed SVO.";
}

void SvoInterface::processImageBundle(
    const std::vector<cv::Mat>& images,
    const int64_t timestamp_nanoseconds)
{
  SVO_WARN_STREAM(">>> [TRACE processImageBundle START] About to call svo_->addImageBundle, stage=" << static_cast<int>(svo_->stage()));
  svo_->addImageBundle(images, timestamp_nanoseconds);
  SVO_WARN_STREAM(">>> [TRACE processImageBundle END] svo_->addImageBundle completed, stage=" << static_cast<int>(svo_->stage()));
}

void SvoInterface::publishResults(
    const std::vector<cv::Mat>& images,
    const int64_t timestamp_nanoseconds)
{
  SVO_WARN_STREAM(">>> [TRACE publishResults START] Stage=" << static_cast<int>(svo_->stage()));
  CHECK_NOTNULL(svo_.get());
  CHECK_NOTNULL(visualizer_.get());

  visualizer_->img_caption_.clear();
  if (svo_->isBackendValid())
  {
    SVO_WARN_STREAM(">>> [TRACE publishResults] Backend is valid, getting stationary status");
    std::string static_str = ceres_backend_interface_->getStationaryStatusStr();
    visualizer_->img_caption_ = static_str;
    SVO_WARN_STREAM(">>> [TRACE publishResults] Got stationary status");
  }

  SVO_WARN_STREAM(">>> [TRACE publishResults] About to publishSvoInfo");
  visualizer_->publishSvoInfo(svo_.get(), timestamp_nanoseconds);
  SVO_WARN_STREAM(">>> [TRACE publishResults] publishSvoInfo completed, entering switch for stage=" << static_cast<int>(svo_->stage()));
  switch (svo_->stage())
  {
    case Stage::kTracking: {
      SVO_WARN_STREAM(">>> [TRACE publishResults] In kTracking case");
      Eigen::Matrix<double, 6, 6> covariance;
      covariance.setZero();
      visualizer_->publishImuPose(
            svo_->getLastFrames()->get_T_W_B(), covariance, timestamp_nanoseconds);
      visualizer_->publishCameraPoses(svo_->getLastFrames(), timestamp_nanoseconds);
      visualizer_->visualizeMarkers(
            svo_->getLastFrames(), svo_->closeKeyframes(), svo_->map());
      visualizer_->exportToDense(svo_->getLastFrames());
      bool draw_boundary = false;
      if (svo_->isBackendValid())
      {
        draw_boundary = svo_->getBundleAdjuster()->isFixedToGlobalMap();
      }
      visualizer_->publishImagesWithFeatures(
            svo_->getLastFrames(), timestamp_nanoseconds,
            draw_boundary);
      SVO_WARN_STREAM(">>> [TRACE publishResults] kTracking case completed");
      break;
    }
    case Stage::kInitializing: {
      SVO_WARN_STREAM(">>> [TRACE publishResults] In kInitializing case, about to publishBundleFeatureTracks");
      visualizer_->publishBundleFeatureTracks(
            svo_->initializer_->frames_ref_, svo_->getLastFrames(),
            timestamp_nanoseconds);
      SVO_WARN_STREAM(">>> [TRACE publishResults] kInitializing case completed");
      break;
    }
    case Stage::kPaused:
    case Stage::kRelocalization:
      SVO_WARN_STREAM(">>> [TRACE publishResults] In kPaused/kRelocalization case");
      visualizer_->publishImages(images, timestamp_nanoseconds);
      break;
    case Stage::kReinitialize:
      SVO_WARN_STREAM(">>> [TRACE publishResults] In kReinitialize case");
      visualizer_->publishImages(images, timestamp_nanoseconds);
      break;
    default:
      LOG(FATAL) << "Unknown stage";
      break;
  }
  SVO_WARN_STREAM(">>> [TRACE publishResults END] Completed all publishing");
}

bool SvoInterface::setImuPrior(const int64_t timestamp_nanoseconds)
{
  if(!svo_->hasStarted())
  {
    // when starting up, make sure we already have IMU measurements
    size_t imu_count = imu_handler_->getMeasurementsCopy().size();
    if(imu_count < 10u)
    {
      VLOG(2) << "Waiting for IMU measurements: " << imu_count << "/10";
      return false;
    }
    VLOG(2) << "IMU prior ready with " << imu_count << " measurements";
  }
  return true;
}

void SvoInterface::monoCallback(const sensor_msgs::msg::Image::ConstSharedPtr& msg)
{
  VLOG(3) << "=== IMAGE CALLBACK RECEIVED === Size: " << msg->width << "x" << msg->height;
  
  if(idle_)
  {
    VLOG(3) << "System is idle, ignoring frame";
    return;
  }

  cv::Mat image;
  try
  {
    image = cv_bridge::toCvCopy(msg)->image;
    VLOG(3) << "Image converted successfully: " << image.cols << "x" << image.rows;
  }
  catch (cv_bridge::Exception& e)
  {
    RCLCPP_ERROR(nh_->get_logger(), "cv_bridge exception: %s", e.what());
  }

  std::vector<cv::Mat> images;
  images.push_back(image.clone());

  int64_t stamp_ns = rclcpp::Time(msg->header.stamp).nanoseconds();
  VLOG(3) << "Image timestamp: " << stamp_ns;
  if(!setImuPrior(stamp_ns))
  {
    VLOG(3) << "Could not align gravity! Attempting again in next iteration.";
    return;
  }

  imageCallbackPreprocessing(stamp_ns);
  processImageBundle(images, stamp_ns);
  publishResults(images, stamp_ns);

  if(svo_->stage() == Stage::kPaused && automatic_reinitialization_)
    svo_->start();

  imageCallbackPostprocessing();
}

void SvoInterface::monoCallbackImage(const sensor_msgs::msg::CompressedImage::ConstSharedPtr& msg)
{
  g_total_images_received++;
  
  try
  {
    VLOG(3) << "=== COMPRESSED IMAGE CALLBACK RECEIVED === Data size: " << msg->data.size() << " bytes";
    SVO_WARN_STREAM(">>> IMAGE REACHED CALLBACK! Total received so far: " << g_total_images_received.load());
    
    if(idle_)
    {
      VLOG(3) << "System is idle, ignoring compressed frame";
      return;
    }

    cv::Mat image;

    image = cv::imdecode(cv::Mat(1, msg->data.size(), CV_8UC1, (void*)msg->data.data()), cv::IMREAD_UNCHANGED);
    VLOG(3) << "Decompressed image: " << image.cols << "x" << image.rows;

    if(image.empty())
    {
      SVO_ERROR_STREAM("Failed to decode compressed image!");
      return;
    }

    std::vector<cv::Mat> images;
    images.push_back(image.clone());

    int64_t stamp_ns = rclcpp::Time(msg->header.stamp).nanoseconds();
    if(!setImuPrior(stamp_ns))
    {
      VLOG(3) << "Could not align gravity! Attempting again in next iteration.";
      return;
    }

    SVO_WARN_STREAM(">>> [TRACE 1] About to call imageCallbackPreprocessing");
    imageCallbackPreprocessing(stamp_ns);
    SVO_WARN_STREAM(">>> [TRACE 2] imageCallbackPreprocessing completed");
    
    SVO_WARN_STREAM(">>> [TRACE 3] About to call processImageBundle");
    processImageBundle(images, stamp_ns);
    SVO_WARN_STREAM(">>> [TRACE 4] processImageBundle completed");
    
    SVO_WARN_STREAM(">>> [TRACE 5] About to call publishResults");
    publishResults(images, stamp_ns);
    SVO_WARN_STREAM(">>> [TRACE 6] publishResults completed");

    if(svo_->stage() == Stage::kPaused && automatic_reinitialization_)
    {
      SVO_WARN_STREAM(">>> [TRACE 7] Starting SVO (stage was paused)");
      svo_->start();
      SVO_WARN_STREAM(">>> [TRACE 8] SVO started");
    }

    SVO_WARN_STREAM(">>> [TRACE 9] About to call imageCallbackPostprocessing");
    imageCallbackPostprocessing();
    SVO_WARN_STREAM(">>> [TRACE 10] imageCallbackPostprocessing completed");
    
    SVO_WARN_STREAM(">>> [TRACE 11] Image callback completed successfully - EXITING CALLBACK");
  }
  catch (const std::exception& e)
  {
    SVO_ERROR_STREAM("EXCEPTION in monoCallbackImage: " << e.what());
  }
  catch (...)
  {
    SVO_ERROR_STREAM("UNKNOWN EXCEPTION in monoCallbackImage!");
  }
}

void SvoInterface::stereoCallback(
    const sensor_msgs::msg::Image::ConstSharedPtr& msg0,
    const sensor_msgs::msg::Image::ConstSharedPtr& msg1)
{
  if(idle_)
    return;

  cv::Mat img0, img1;
  try {
    img0 = cv_bridge::toCvShare(msg0, "mono8")->image;
    img1 = cv_bridge::toCvShare(msg1, "mono8")->image;
  } catch (cv_bridge::Exception& e) {
    RCLCPP_ERROR(nh_->get_logger(), "cv_bridge exception: %s", e.what());
  }

  int64_t stamp_ns = rclcpp::Time(msg0->header.stamp).nanoseconds();
  if(!setImuPrior(stamp_ns))
  {
    VLOG(3) << "Could not align gravity! Attempting again in next iteration.";
    return;
  }

  imageCallbackPreprocessing(stamp_ns);
  processImageBundle({img0, img1}, stamp_ns);
  publishResults({img0, img1}, stamp_ns);

  if(svo_->stage() == Stage::kPaused && automatic_reinitialization_)
    svo_->start();

  imageCallbackPostprocessing();
}

void SvoInterface::imuCallback(const sensor_msgs::msg::Imu::ConstSharedPtr& msg)
{
  static int imu_callback_count = 0;
  imu_callback_count++;
  if (imu_callback_count % 50 == 0) {
    RCLCPP_INFO(nh_->get_logger(), ">>> IMU CALLBACK RECEIVED! Count=%d, timestamp=%.6f", 
                imu_callback_count, rclcpp::Time(msg->header.stamp).seconds());
  }
  
  const Eigen::Vector3d omega_imu(
        msg->angular_velocity.x, msg->angular_velocity.y, msg->angular_velocity.z);
  const Eigen::Vector3d lin_acc_imu(
        msg->linear_acceleration.x, msg->linear_acceleration.y, msg->linear_acceleration.z);

  // correct for scale and axis misalignment
  if (imu_handler_->imu_calib_.correct_scale_and_axis_misalignment)
  {
    Eigen::Vector3d omega_imu_corr = imu_handler_->imu_calib_.Mg_inv * omega_imu;
    Eigen::Vector3d lin_acc_imu_corr = imu_handler_->imu_calib_.Ma_inv * lin_acc_imu; 

    const ImuMeasurement m(rclcpp::Time(msg->header.stamp).seconds(), omega_imu_corr, lin_acc_imu_corr);
    if(imu_handler_)
      imu_handler_->addImuMeasurement(m);
    else
      SVO_ERROR_STREAM("SvoNode has no ImuHandler");
  }
  else
  {
    const ImuMeasurement m(rclcpp::Time(msg->header.stamp).seconds(), omega_imu, lin_acc_imu);
    if(imu_handler_)
      imu_handler_->addImuMeasurement(m);
    else
      SVO_ERROR_STREAM("SvoNode has no ImuHandler");
  }
}

void SvoInterface::dynamicsCallback(const agiros_msgs::msg::Command::ConstSharedPtr& msg)
{
  Eigen::Vector3d mass_norm_collective_thrust(Eigen::Vector3d::Zero());
  Eigen::Vector3d collective_torque(Eigen::Vector3d::Zero());
  Eigen::Vector3d collective_torque_in_imu_frame(Eigen::Vector3d::Zero());
  DynamicsMeasurement m;

  // Agilicious
  if (dynamics_dataset_ == 0)
  {
    // mass-normalized collective thrust
    double fz, fz_1, fz_2, fz_3, fz_4;
    fz_1 = msg->thrusts[0];
    fz_2 = msg->thrusts[1];
    fz_3 = msg->thrusts[2];
    fz_4 = msg->thrusts[3];

    fz = fz_1 + fz_2 + fz_3 + fz_4;
    fz /= dynamics_handler_->options_.mass;

    mass_norm_collective_thrust(2) = fz;

    double omega2_1, omega2_2, omega2_3, omega2_4;
    omega2_1 = fz_1 / dynamics_handler_->options_.thrust_coeffs(0);
    omega2_2 = fz_2 / dynamics_handler_->options_.thrust_coeffs(1);
    omega2_3 = fz_3 / dynamics_handler_->options_.thrust_coeffs(2);
    omega2_4 = fz_4 / dynamics_handler_->options_.thrust_coeffs(3);

    double tau_1, tau_2, tau_3, tau_4;
    tau_1 = dynamics_handler_->options_.torque_coeffs(0) * omega2_1;
    tau_2 = dynamics_handler_->options_.torque_coeffs(1) * omega2_2;
    tau_3 = dynamics_handler_->options_.torque_coeffs(2) * omega2_3;
    tau_4 = dynamics_handler_->options_.torque_coeffs(3) * omega2_4;

    collective_torque += Eigen::Vector3d(0., 0., tau_3);
    collective_torque += Eigen::Vector3d(0., 0., tau_4);
    collective_torque -= Eigen::Vector3d(0., 0., tau_1);
    collective_torque -= Eigen::Vector3d(0., 0., tau_2);

    // r_i x thrust_i component
    collective_torque += skew(dynamics_handler_->options_.r_br) * Eigen::Vector3d(0., 0., fz_1);
    collective_torque += skew(dynamics_handler_->options_.r_fl) * Eigen::Vector3d(0., 0., fz_2);
    collective_torque += skew(dynamics_handler_->options_.r_bl) * Eigen::Vector3d(0., 0., fz_3);
    collective_torque += skew(dynamics_handler_->options_.r_fl) * Eigen::Vector3d(0., 0., fz_2);

    m.timestamp_ = rclcpp::Time(msg->header.stamp).seconds();
    m.collective_thrust_ = mass_norm_collective_thrust;
    m.collective_torque_ = collective_torque;

  }
  // Blackbird
  else if (dynamics_dataset_ == 1)
  {
    double omega2_br, omega2_fr, omega2_bl, omega2_fl;
    omega2_br = pow(msg->thrusts[3], 2);
    omega2_fr = pow(msg->thrusts[1], 2);
    omega2_bl = pow(msg->thrusts[2], 2);
    omega2_fl = pow(msg->thrusts[0], 2);
    
    // mass-normalized collective thrust
    double fz, fz_br, fz_fr, fz_bl, fz_fl;
    fz_br = dynamics_handler_->options_.thrust_coeffs(3) * omega2_br;
    fz_fr = dynamics_handler_->options_.thrust_coeffs(1) * omega2_fr;
    fz_bl = dynamics_handler_->options_.thrust_coeffs(2) * omega2_bl;
    fz_fl = dynamics_handler_->options_.thrust_coeffs(0) * omega2_fl;

    fz = fz_bl + fz_br + fz_fl + fz_fr;
    fz /= dynamics_handler_->options_.mass;

    // z-axis is pointing downwards
    mass_norm_collective_thrust(2) = -1. * fz;

    // collective torque
    // br = motor 4, fr = motor 2, bl = motor 3, fl = motor 1
    // c_t x omega_i^2 component
    double tau_br, tau_fr, tau_bl, tau_fl;
    tau_br = dynamics_handler_->options_.torque_coeffs(3) * omega2_br;
    tau_fr = dynamics_handler_->options_.torque_coeffs(1) * omega2_fr;
    tau_bl = dynamics_handler_->options_.torque_coeffs(2) * omega2_bl;
    tau_fl = dynamics_handler_->options_.torque_coeffs(0) * omega2_fl;
    
    collective_torque += Eigen::Vector3d(0., 0., tau_fr);
    collective_torque += Eigen::Vector3d(0., 0., tau_bl);
    collective_torque -= Eigen::Vector3d(0., 0., tau_br);
    collective_torque -= Eigen::Vector3d(0., 0., tau_fl);

    // r_i x thrust_i component
    // -1. it's because the z-axis points down
    collective_torque += skew(dynamics_handler_->options_.r_br) * Eigen::Vector3d(0., 0., -1.*fz_br);
    collective_torque += skew(dynamics_handler_->options_.r_fr) * Eigen::Vector3d(0., 0., -1.*fz_fr);
    collective_torque += skew(dynamics_handler_->options_.r_bl) * Eigen::Vector3d(0., 0., -1.*fz_bl);
    collective_torque += skew(dynamics_handler_->options_.r_fl) * Eigen::Vector3d(0., 0., -1.*fz_fl);

    // Transform to IMU frame
    Eigen::Matrix4d T_B_I(ncam_->get_T_B_I().getTransformationMatrix());
    Eigen::Matrix3d R_B_I(T_B_I.block(0,0,3,3));

    collective_torque_in_imu_frame = R_B_I.inverse() * collective_torque;

    m.timestamp_ = rclcpp::Time(msg->header.stamp).seconds();
    m.collective_thrust_ = mass_norm_collective_thrust;
    m.collective_torque_ = collective_torque_in_imu_frame;
  }
  // VID
  else if (dynamics_dataset_ == 2)
  {
    SVO_ERROR_STREAM("Not implemented yet!");
  }
  else
  {
    SVO_ERROR_STREAM("Unknown dynamics dataset!");
  }
  
  if(dynamics_handler_)
  {
    dynamics_handler_->addDynamicsMeasurement(m);
  }
  else
  {
    SVO_ERROR_STREAM("SvoNode has no DynamicsHandler");
  } 
}

void SvoInterface::debugDynamicsCallback(const geometry_msgs::msg::WrenchStamped::ConstSharedPtr &msg)
{
  Eigen::Vector3d thrust(Eigen::Vector3d::Zero());
  Eigen::Vector3d torque(Eigen::Vector3d::Zero());

  // Blackbird
  if (dynamics_dataset_ == 1)
  {
    thrust(0) = msg->wrench.force.x;
    thrust(1) = msg->wrench.force.y;
    thrust(2) = msg->wrench.force.z;

    torque(0) = msg->wrench.torque.x;
    torque(1) = msg->wrench.torque.y;
    torque(2) = msg->wrench.torque.z;
  }
  else
  {
    SVO_ERROR_STREAM("Debug Dynamics not implemented for this dataset!");
  }

  DynamicsMeasurement m;
  m.timestamp_ = rclcpp::Time(msg->header.stamp).seconds();
  m.collective_thrust_ = thrust;
  m.collective_torque_ = torque;
  
  if(dynamics_handler_)
  {
    dynamics_handler_->addDynamicsMeasurement(m);
  }
  else
  {
    SVO_ERROR_STREAM("SvoNode has no DynamicsHandler");
  }

}

void SvoInterface::inputKeyCallback(const std_msgs::msg::String::ConstSharedPtr& key_input)
{
  std::string remote_input = key_input->data;
  char input = remote_input.c_str()[0];
  switch(input)
  {
    case 'q':
      quit_ = true;
      SVO_INFO_STREAM("SVO user input: QUIT");
      break;
    case 'r':
      svo_->reset();
      idle_ = true;
      SVO_INFO_STREAM("SVO user input: RESET");
      break;
    case 's':
      svo_->start();
      idle_ = false;
      SVO_INFO_STREAM("SVO user input: START");
      break;
     case 'c':
      svo_->setCompensation(true);
      SVO_INFO_STREAM("Enabled affine compensation.");
      break;
     case 'C':
      svo_->setCompensation(false);
      SVO_INFO_STREAM("Disabled affine compensation.");
      break;
    default: ;
  }
}

void SvoInterface::subscribeImu()
{
  imu_thread_ = std::unique_ptr<std::thread>(
        new std::thread(&SvoInterface::imuLoop, this));
}

void SvoInterface::subscribeDynamics()
{
  dynamics_thread_ = std::unique_ptr<std::thread>(
        new std::thread(&SvoInterface::dynamicsLoop, this));
}

void SvoInterface::subscribeDebugDynamics()
{
  dynamics_debug_thread_ = std::unique_ptr<std::thread>(
        new std::thread(&SvoInterface::debugDynamicsLoop, this));
}

void SvoInterface::subscribeImage()
{
  if(pipeline_type_ == PipelineType::kMono)
    image_thread_ = std::unique_ptr<std::thread>(
          new std::thread(&SvoInterface::monoLoop, this));
  else if(pipeline_type_ == PipelineType::kStereo)
    image_thread_ = std::unique_ptr<std::thread>(
        new std::thread(&SvoInterface::stereoLoop, this));
}

void SvoInterface::subscribeRemoteKey()
{
  std::string remote_key_topic =
      pnh_->declare_parameter<std::string>("remote_key_topic", "svo/remote_key");
  sub_remote_key_ =
      nh_->create_subscription<std_msgs::msg::String>(
          remote_key_topic, 5,
          std::bind(&svo::SvoInterface::inputKeyCallback, this, std::placeholders::_1));
}

void SvoInterface::imuLoop()
{
  SVO_INFO_STREAM("SvoNode: Started IMU loop.");
  std::string imu_topic = pnh_->declare_parameter<std::string>("imu_topic", "imu");
  VLOG(1) << "IMU subscribing to: " << imu_topic;
  
  // Use BEST_EFFORT QoS to match PX4's IMU publisher
  auto qos = rclcpp::QoS(rclcpp::KeepLast(10));
  qos.best_effort();
  
  sub_imu_ = nh_->create_subscription<sensor_msgs::msg::Imu>(
      imu_topic, qos,
      std::bind(&svo::SvoInterface::imuCallback, this, std::placeholders::_1));
  
  int loop_count = 0;
  while(rclcpp::ok() && !quit_)
  {
    if (loop_count % 100 == 0)  // Log every second
    {
      size_t imu_count = imu_handler_ ? imu_handler_->getMeasurementsCopy().size() : 0;
      VLOG(2) << "IMU loop alive, iteration: " << loop_count << ", buffered IMU msgs: " << imu_count;
    }
    loop_count++;
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  SVO_INFO_STREAM("SvoNode: IMU loop exiting.");
}

void SvoInterface::dynamicsLoop()
{
  SVO_INFO_STREAM("SvoNode: Started Dynamics loop.");
  std::string dynamics_topic = pnh_->declare_parameter<std::string>("dynamics_topic", "dynamics");
  sub_dynamics_ = nh_->create_subscription<agiros_msgs::msg::Command>(
      dynamics_topic, 10,
      std::bind(&svo::SvoInterface::dynamicsCallback, this, std::placeholders::_1));
  
  int loop_count = 0;
  while(rclcpp::ok() && !quit_)
  {
    if (loop_count % 100 == 0)  // Log every second
    {
      VLOG(2) << "Dynamics loop alive, iteration: " << loop_count;
    }
    loop_count++;
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  SVO_INFO_STREAM("SvoNode: Dynamics loop exiting.");
}

void SvoInterface::debugDynamicsLoop()
{
  SVO_INFO_STREAM("SvoNode: Started Debug Dynamics loop.");
  std::string dynamics_topic = pnh_->declare_parameter<std::string>("dynamics_topic", "dynamics");
  sub_debug_dynamics_ = nh_->create_subscription<geometry_msgs::msg::WrenchStamped>(
      dynamics_topic, 10,
      std::bind(&svo::SvoInterface::debugDynamicsCallback, this, std::placeholders::_1));
  while(rclcpp::ok() && !quit_)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}

void SvoInterface::monoLoop()
{
  SVO_INFO_STREAM("SvoNode: Started Image loop.");

  std::string image_topic =
      pnh_->declare_parameter<std::string>("cam0_topic", "camera/image_raw");
  
  // Subscribe directly to compressed images
  std::string compressed_topic = image_topic + "/compressed";
  VLOG(1) << "Subscribing to compressed image topic: " << compressed_topic;
  
  sub_compressed_image_ = nh_->create_subscription<sensor_msgs::msg::CompressedImage>(
      compressed_topic, 10,
      std::bind(&svo::SvoInterface::monoCallbackImage, this, std::placeholders::_1));

  int loop_count = 0;
  int last_image_count = 0;
  auto last_check_time = std::chrono::steady_clock::now();
  
  while(rclcpp::ok() && !quit_)
  {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - last_check_time).count();
    
    if (elapsed >= 2)  // Check every 2 seconds
    {
      int current_count = g_total_images_received.load();
      int images_received_in_period = current_count - last_image_count;
      
      if (images_received_in_period == 0)
      {
        SVO_ERROR_STREAM("!!! WARNING: NO IMAGES RECEIVED IN LAST " << elapsed << " SECONDS !!! Total ever received: " << current_count);
      }
      else
      {
        SVO_INFO_STREAM("Images received in last " << elapsed << "s: " << images_received_in_period << " (rate: " << (images_received_in_period / (float)elapsed) << " Hz)");
      }
      
      last_image_count = current_count;
      last_check_time = now;
    }
    
    if (loop_count % 100 == 0)  // Log every second (100 * 10ms = 1s)
    {
      VLOG(2) << "Image loop alive, iteration: " << loop_count << ", Stage: " << static_cast<int>(svo_->stage());
    }
    loop_count++;
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  SVO_INFO_STREAM("SvoNode: Image loop exiting.");
}

void SvoInterface::stereoLoop()
{
  typedef message_filters::sync_policies::ExactTime<sensor_msgs::msg::Image, sensor_msgs::msg::Image> ExactPolicy;
  typedef message_filters::Synchronizer<ExactPolicy> ExactSync;

  // subscribe to cam msgs
  std::string cam0_topic(pnh_->declare_parameter<std::string>("cam0_topic", "/cam0/image_raw"));
  std::string cam1_topic(pnh_->declare_parameter<std::string>("cam1_topic", "/cam1/image_raw"));
  
  message_filters::Subscriber<sensor_msgs::msg::Image> sub0(nh_, cam0_topic, rmw_qos_profile_sensor_data);
  message_filters::Subscriber<sensor_msgs::msg::Image> sub1(nh_, cam1_topic, rmw_qos_profile_sensor_data);
  
  std::shared_ptr<ExactSync> sync_sub = std::make_shared<ExactSync>(ExactPolicy(5), sub0, sub1);
  sync_sub->registerCallback(std::bind(&svo::SvoInterface::stereoCallback, this,
                                       std::placeholders::_1, std::placeholders::_2));

  while(rclcpp::ok() && !quit_)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}

} // namespace svo
