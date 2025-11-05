// ============================================================================
// HDVIO2 Node - Modern ROS 2 Implementation
// Proper use of MultiThreadedExecutor with callback groups
// ============================================================================

#include "hdvio2/svo_node_base.h"

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <rclcpp/rclcpp.hpp>
#include <svo/common/logging.h>
#include <vikit/params_helper.h>

namespace svo_ros {

void SvoNodeBase::initThirdParty(int argc, char **argv)
{
  // Initialize ROS2 first to handle --ros-args
  rclcpp::init(argc, argv);
  
  google::InitGoogleLogging(argv[0]);
  google::InstallFailureSignalHandler();
}

SvoNodeBase::SvoNodeBase()
: node_handle_(std::make_shared<rclcpp::Node>("svo")),
  private_node_handle_(node_handle_),
  type_(private_node_handle_->declare_parameter("pipeline_is_stereo", false) ?
        svo::PipelineType::kStereo : svo::PipelineType::kMono),
  svo_interface_(type_, node_handle_, private_node_handle_)
{
  SVO_INFO_STREAM("=== HDVIO2 Node Initialized ===");
  SVO_INFO_STREAM("Pipeline type: " << 
      (type_ == svo::PipelineType::kMono ? "MONO" : "STEREO"));
}

void SvoNodeBase::run()
{
  SVO_INFO_STREAM("Starting modern ROS 2 MultiThreadedExecutor...");
  
  // Create multi-threaded executor with multiple threads
  // This allows callback groups to execute in parallel
  int num_threads = private_node_handle_->declare_parameter("executor_threads", 4);
  rclcpp::executors::MultiThreadedExecutor executor(
      rclcpp::ExecutorOptions(), num_threads);
  
  executor.add_node(node_handle_);
  
  SVO_INFO_STREAM("Executor started with " << num_threads << " threads");
  SVO_INFO_STREAM("All subscriptions are active and ready to receive data");
  
  // Spin the executor (this will handle all callbacks)
  // Each callback group can execute in parallel on different threads
  try {
    executor.spin();  // Blocks until shutdown
  } catch (const std::exception& e) {
    SVO_ERROR_STREAM("Exception during executor spin: " << e.what());
  }
  
  SVO_INFO_STREAM("Executor stopped - shutting down HDVIO2");
  SVO_INFO_STREAM("SVO terminated.");
}

}  // namespace svo_ros
