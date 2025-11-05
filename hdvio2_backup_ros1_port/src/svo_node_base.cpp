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
  // Don't parse command line flags in ROS2 - let rclcpp handle arguments
  google::InstallFailureSignalHandler();
}

SvoNodeBase::SvoNodeBase()
: node_handle_(std::make_shared<rclcpp::Node>("svo")),
  private_node_handle_(node_handle_),
  type_(private_node_handle_->declare_parameter("pipeline_is_stereo", false) ?
        svo::PipelineType::kStereo : svo::PipelineType::kMono),
  svo_interface_(type_, node_handle_, private_node_handle_)
{
  svo_interface_.subscribeImu();

  svo_interface_.subscribeDynamics();
  
  // Function that subscribes to debug (ground truth) dynamics
  // svo_interface_.subscribeDebugDynamics();
  
  svo_interface_.subscribeImage();
}

void SvoNodeBase::run()
{
  SVO_INFO_STREAM("Starting main ROS2 spin loop...");
  
  // Use MultiThreadedExecutor to handle callbacks
  rclcpp::executors::MultiThreadedExecutor executor;
  executor.add_node(node_handle_);
  
  // Spin with periodic status logging
  int iteration = 0;
  int log_counter = 0;
  while (rclcpp::ok() && !svo_interface_.quit_)
  {
    try
    {
      executor.spin_some(std::chrono::milliseconds(100));
    }
    catch (const std::exception& e)
    {
      SVO_ERROR_STREAM("Exception in spin_some: " << e.what());
    }
    
    iteration++;
    log_counter++;
    
    // Log every 20 iterations (20 * 100ms = 2 seconds)
    if (log_counter >= 20)
    {
      SVO_INFO_STREAM("Main ROS2 executor loop alive, iter=" << iteration 
                      << " rclcpp::ok()=" << rclcpp::ok() 
                      << " quit=" << svo_interface_.quit_);
      log_counter = 0;
    }
  }
  
  SVO_INFO_STREAM("SVO quit - loop exited! rclcpp::ok()=" << rclcpp::ok() 
                  << " quit=" << svo_interface_.quit_);
  svo_interface_.quit_ = true;
  SVO_INFO_STREAM("SVO terminated.\n");
}

}  // namespace svo_ros
