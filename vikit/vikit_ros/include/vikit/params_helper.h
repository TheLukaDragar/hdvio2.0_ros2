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
    try
    {
      T v = node->declare_parameter(name, defaultValue);
      RCLCPP_INFO_STREAM(node->get_logger(), "Declared parameter: " << name << ", value: " << v);
      return v;
    }
    catch(const rclcpp::exceptions::ParameterAlreadyDeclaredException&)
    {
      T v = node->get_parameter(name).get_value<T>();
      RCLCPP_INFO_STREAM(node->get_logger(), "Found parameter: " << name << ", value: " << v);
      return v;
    }
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
    RCLCPP_ERROR_STREAM(node->get_logger(), "Parameter not declared and no default provided: " << name);
    return T();
  }
}

template<typename T>
T param(rclcpp::Node::SharedPtr node, const std::string& name, const T& defaultValue,
        const bool silent=false)
{
  try
  {
    // First try to get the parameter if it exists
    T v = node->get_parameter(name).get_value<T>();
    if (!silent)
    {
      RCLCPP_INFO_STREAM(node->get_logger(), "Found parameter: " << name << ", value: " << v);
    }
    return v;
  }
  catch(const rclcpp::exceptions::ParameterNotDeclaredException&)
  {
    // Parameter doesn't exist, declare it with the default value
    try
    {
      T v = node->declare_parameter(name, defaultValue);
      if (!silent)
      {
        RCLCPP_INFO_STREAM(node->get_logger(), "Declared parameter: " << name << ", value: " << v);
      }
      return v;
    }
    catch(const rclcpp::exceptions::ParameterAlreadyDeclaredException&)
    {
      // Parameter was declared by another thread, get its value
      T v = node->get_parameter(name).get_value<T>();
      if (!silent)
      {
        RCLCPP_INFO_STREAM(node->get_logger(), "Found parameter: " << name << ", value: " << v);
      }
      return v;
    }
  }
  catch(const rclcpp::ParameterTypeException&)
  {
    // Type mismatch - parameter exists but with different type
    // Try to get it as the existing type and convert if possible
    if (!silent)
    {
      RCLCPP_WARN_STREAM(node->get_logger(), "Parameter " << name << " exists with different type, using default: " << defaultValue);
    }
    return defaultValue;
  }
}

} // namespace vk

#endif // ROS_PARAMS_HELPER_H_
