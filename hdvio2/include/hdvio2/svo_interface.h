#pragma once

#include <thread>

#include <ros/ros.h>
#include <std_msgs/String.h>    // user-input
#include <geometry_msgs/WrenchStamped.h>
#include <sensor_msgs/Image.h>
#include <sensor_msgs/CompressedImage.h>
#include <sensor_msgs/Imu.h>

#include <svo/common/types.h>
#include <svo/common/camera_fwd.h>
#include <svo/common/transformation.h>

#include <opencv2/imgcodecs.hpp>

#include "svo_msgs/Command.h"

namespace svo {

// forward declarations
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

/// SVO Interface
class SvoInterface
{
public:

  // ROS subscription and publishing.
  ros::NodeHandle nh_;
  ros::NodeHandle pnh_;
  PipelineType pipeline_type_;
  ros::Subscriber sub_remote_key_;
  std::string remote_input_;
  std::unique_ptr<std::thread> imu_thread_;
  std::unique_ptr<std::thread> dynamics_thread_;
  std::unique_ptr<std::thread> dynamics_debug_thread_;
  std::unique_ptr<std::thread> image_thread_;

  // SVO modules.
  std::shared_ptr<FrameHandlerBase> svo_;
  std::shared_ptr<Visualizer> visualizer_;
  std::shared_ptr<ImuHandler> imu_handler_;
  std::shared_ptr<DynamicsHandler> dynamics_handler_;
  std::shared_ptr<DynamicsNet> thrust_dynamics_net_;
  std::shared_ptr<DynamicsNet> torque_dynamics_net_;
  std::shared_ptr<OptimizeQuadAngVelSpline> opt_quad_angvel_spline_;
  std::shared_ptr<BackendInterface> backend_interface_;
  std::shared_ptr<CeresBackendInterface> ceres_backend_interface_;
  std::shared_ptr<CeresBackendPublisher> ceres_backend_publisher_;

  CameraBundlePtr ncam_;

  // Parameters
  bool set_initial_attitude_from_gravity_ = true;

  // System state.
  bool quit_ = false;
  bool idle_ = false;
  bool automatic_reinitialization_ = false;

  // Dynamics dataset
  // 0 (default) : agilicious
  // 1 : Blackbird
  // 2 : VID
  int dynamics_dataset_ = 0;

  SvoInterface(const PipelineType& pipeline_type,
          const ros::NodeHandle& nh,
          const ros::NodeHandle& private_nh);

  virtual ~SvoInterface();

  // Processing
  void processImageBundle(
      const std::vector<cv::Mat>& images,
      int64_t timestamp_nanoseconds);

  bool setImuPrior(const int64_t timestamp_nanoseconds);

  void publishResults(
      const std::vector<cv::Mat>& images,
      const int64_t timestamp_nanoseconds);

  // Subscription and callbacks
  void monoCallback(const sensor_msgs::ImageConstPtr& msg);
  void monoCallbackImage(const sensor_msgs::CompressedImage& msg);
  void stereoCallback(
      const sensor_msgs::ImageConstPtr& msg0,
      const sensor_msgs::ImageConstPtr& msg1);
  void imuCallback(const sensor_msgs::ImuConstPtr& imu_msg);
  void dynamicsCallback(const svo_msgs::CommandConstPtr &dynamics_msg);
  void debugDynamicsCallback(const geometry_msgs::WrenchStampedConstPtr &dynamics_msg);
  void inputKeyCallback(const std_msgs::StringConstPtr& key_input);


  // These functions are called before and after monoCallback or stereoCallback.
  // a derived class can implement some additional logic here.
  virtual void imageCallbackPreprocessing(int64_t timestamp_nanoseconds) {}
  virtual void imageCallbackPostprocessing() {}

  void subscribeImu();
  void subscribeDynamics();
  void subscribeDebugDynamics();
  void subscribeImage();
  void subscribeRemoteKey();

  void imuLoop();
  void dynamicsLoop();
  void debugDynamicsLoop();
  void monoLoop();
  void stereoLoop();
};

} // namespace svo
