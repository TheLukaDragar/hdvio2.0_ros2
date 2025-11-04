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
  rclcpp::spin(node_handle_);
  SVO_INFO_STREAM("SVO quit");
  svo_interface_.quit_ = true;
  SVO_INFO_STREAM("SVO terminated.\n");
}

}  // namespace svo_ros
