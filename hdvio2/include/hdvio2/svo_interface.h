#pragma once

#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>

#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>
#include <geometry_msgs/msg/wrench_stamped.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <sensor_msgs/msg/imu.hpp>
#include <image_transport/image_transport.hpp>
#include <cv_bridge/cv_bridge.h>

#include <svo/common/types.h>
#include <svo/common/camera_fwd.h>
#include <svo/common/transformation.h>

#include "agiros_msgs/msg/command.hpp"

namespace svo {

// Forward declarations
class FrameHandlerBase;
class Visualizer;
class ImuHandler;
class DynamicsHandler;
class DynamicsNet;
class OptimizeQuadAngVelSpline;
class BackendInterface;
class CeresBackendInterface;
class CeresBackendPublisher;

enum class PipelineType {
  kMono,
  kStereo,
  kArray
};

/// Image data for processing queue
struct ImageData {
  cv::Mat image;
  int64_t timestamp_ns;
  
  ImageData() = default;
  ImageData(const cv::Mat& img, int64_t ts) 
    : image(img), timestamp_ns(ts) {}
};

/// Modern ROS 2 SVO Interface with proper callback groups and worker threads
class SvoInterface
{
public:
  SvoInterface(
      const PipelineType& pipeline_type,
      const rclcpp::Node::SharedPtr& nh,
      const rclcpp::Node::SharedPtr& private_nh);

  virtual ~SvoInterface();

  // System state accessors
  bool quit() const { return quit_.load(); }
  void requestShutdown() { quit_.store(true); }

protected:
  // ============================================================================
  // Protected members for derived classes (benchmark nodes)
  // ============================================================================
  rclcpp::Node::SharedPtr nh_;
  rclcpp::Node::SharedPtr pnh_;
  PipelineType pipeline_type_;
  std::shared_ptr<FrameHandlerBase> svo_;
  std::shared_ptr<Visualizer> visualizer_;
  std::shared_ptr<ImuHandler> imu_handler_;
  std::shared_ptr<DynamicsHandler> dynamics_handler_;
  CameraBundlePtr ncam_;
  
  // Parameters accessible to derived classes
  int dynamics_dataset_ = 0;  // 0=agilicious, 1=blackbird, 2=VID

  // Processing functions (protected so benchmark nodes can access them)
  void processImageBundle(const std::vector<cv::Mat>& images, int64_t timestamp_ns);
  void publishResults(const std::vector<cv::Mat>& images, int64_t timestamp_ns);
  bool setImuPrior(int64_t timestamp_ns);

  // Virtual hooks for derived classes
  virtual void imageCallbackPreprocessing(int64_t timestamp_ns) {}
  virtual void imageCallbackPostprocessing() {}

private:
  // ============================================================================
  // ROS 2 Components
  // ============================================================================
  
  // Callback groups for parallel execution
  rclcpp::CallbackGroup::SharedPtr image_callback_group_;
  rclcpp::CallbackGroup::SharedPtr imu_callback_group_;
  rclcpp::CallbackGroup::SharedPtr dynamics_callback_group_;
  rclcpp::CallbackGroup::SharedPtr control_callback_group_;

  // Subscriptions
  std::shared_ptr<image_transport::ImageTransport> image_transport_;
  image_transport::Subscriber sub_image_;
  rclcpp::Subscription<sensor_msgs::msg::Imu>::SharedPtr sub_imu_;
  rclcpp::Subscription<agiros_msgs::msg::Command>::SharedPtr sub_dynamics_;
  rclcpp::Subscription<geometry_msgs::msg::WrenchStamped>::SharedPtr sub_debug_dynamics_;
  rclcpp::Subscription<std_msgs::msg::String>::SharedPtr sub_remote_key_;

  // Backend and other modules (private)
  std::shared_ptr<DynamicsNet> thrust_dynamics_net_;
  std::shared_ptr<DynamicsNet> torque_dynamics_net_;
  std::shared_ptr<OptimizeQuadAngVelSpline> opt_quad_angvel_spline_;
  std::shared_ptr<BackendInterface> backend_interface_;
  std::shared_ptr<CeresBackendInterface> ceres_backend_interface_;
  std::shared_ptr<CeresBackendPublisher> ceres_backend_publisher_;

  // ============================================================================
  // Image Processing Queue and Worker Thread
  // ============================================================================
  std::queue<ImageData> image_queue_;
  std::mutex image_queue_mutex_;
  std::condition_variable image_queue_cv_;
  std::thread image_processing_thread_;
  std::atomic<bool> quit_{false};
  std::atomic<bool> idle_{false};
  std::atomic<int> total_images_received_{0};
  
  size_t max_queue_size_ = 5;  // Drop frames if queue grows too large

  // ============================================================================
  // Parameters
  // ============================================================================
  bool set_initial_attitude_from_gravity_ = true;
  bool automatic_reinitialization_ = false;

  // ============================================================================
  // Fast ROS 2 Callbacks (must return quickly!)
  // ============================================================================
  void imageCallbackFast(const sensor_msgs::msg::Image::ConstSharedPtr& msg);
  void imuCallbackFast(const sensor_msgs::msg::Imu::ConstSharedPtr& msg);
  void dynamicsCallbackFast(const agiros_msgs::msg::Command::ConstSharedPtr& msg);
  void debugDynamicsCallbackFast(const geometry_msgs::msg::WrenchStamped::ConstSharedPtr& msg);
  void inputKeyCallback(const std_msgs::msg::String::ConstSharedPtr& key_input);

  // ============================================================================
  // Worker Thread for Heavy Processing
  // ============================================================================
  void imageProcessingWorker();

  // ============================================================================
  // Initialization
  // ============================================================================
  void setupSubscriptions();
  void initializeSvoModules();
};

} // namespace svo
