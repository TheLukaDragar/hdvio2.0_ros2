/*
 * ros_params_helper.h
 *
 *  Created on: Feb 22, 2013
 *      Author: cforster
 *
 * from libpointmatcher_ros
 * Migrated to ROS2
 */

#ifndef ROS_PARAMS_HELPER_H_
#define ROS_PARAMS_HELPER_H_

#include <string>
#include <rclcpp/rclcpp.hpp>

namespace vk {

inline
bool hasParam(rclcpp::Node::SharedPtr node, const std::string& name)
{
  return node->has_parameter(name);
}

template<typename T>
T getParam(rclcpp::Node::SharedPtr node, const std::string& name, const T& defaultValue)
{
  try
  {
    T v = node->get_parameter(name).get_value<T>();
    RCLCPP_INFO_STREAM(node->get_logger(), "Found parameter: " << name << ", value: " << v);
    return v;
  }
  catch(const rclcpp::exceptions::ParameterNotDeclaredException&)
  {
    RCLCPP_WARN_STREAM(node->get_logger(), "Cannot find value for parameter: " << name << ", assigning default: " << defaultValue);
    return defaultValue;
  }
}

template<typename T>
T getParam(rclcpp::Node::SharedPtr node, const std::string& name)
{
  try
  {
    T v = node->get_parameter(name).get_value<T>();
    RCLCPP_INFO_STREAM(node->get_logger(), "Found parameter: " << name << ", value: " << v);
    return v;
  }
  catch(const rclcpp::exceptions::ParameterNotDeclaredException&)
  {
    RCLCPP_ERROR_STREAM(node->get_logger(), "Cannot find value for parameter: " << name);
    return T();
  }
}

template<typename T>
T param(rclcpp::Node::SharedPtr node, const std::string& name, const T& defaultValue,
        const bool silent=false)
{
  if(node->has_parameter(name))
  {
    T v = node->get_parameter(name).get_value<T>();
    if (!silent)
    {
      RCLCPP_INFO_STREAM(node->get_logger(), "Found parameter: " << name << ", value: " << v);
    }
    return v;
  }
  if (!silent)
  {
    RCLCPP_WARN_STREAM(node->get_logger(), "Cannot find value for parameter: " << name << ", assigning default: " << defaultValue);
  }
  return defaultValue;
}

} // namespace vk

#endif // ROS_PARAMS_HELPER_H_
