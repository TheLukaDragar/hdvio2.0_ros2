#pragma once

#include "hdvio2/svo_interface.h"

namespace svo_ros {

class SvoNodeBase {
 public:
  // Initializes glog, gflags and ROS.
  static void initThirdParty(int argc, char **argv);

  SvoNodeBase();

  void run();

 private:
  rclcpp::Node::SharedPtr node_handle_;
  rclcpp::Node::SharedPtr private_node_handle_;
  svo::PipelineType type_;

 public:
   svo::SvoInterface svo_interface_;
};

}  // namespace svo_ros
