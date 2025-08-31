#include "hdvio2/svo_node_base.h"

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <ros/ros.h>
#include <svo/common/logging.h>
#include <vikit/params_helper.h>

namespace svo_ros {

void SvoNodeBase::initThirdParty(int argc, char **argv)
{
  google::InitGoogleLogging(argv[0]);
  google::ParseCommandLineFlags(&argc, &argv, true);
  google::InstallFailureSignalHandler();

  ros::init(argc, argv, "svo");
}

SvoNodeBase::SvoNodeBase()
: node_handle_(), private_node_handle_("~"), type_(
    vk::param<bool>(private_node_handle_, "pipeline_is_stereo", false) ?
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
  ros::spin();
  SVO_INFO_STREAM("SVO quit");
  svo_interface_.quit_ = true;
  SVO_INFO_STREAM("SVO terminated.\n");
}

}  // namespace svo_ros
