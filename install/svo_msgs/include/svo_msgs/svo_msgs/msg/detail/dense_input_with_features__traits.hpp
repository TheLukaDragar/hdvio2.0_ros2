// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from svo_msgs:msg/DenseInputWithFeatures.idl
// generated code does not contain a copyright notice

#ifndef SVO_MSGS__MSG__DETAIL__DENSE_INPUT_WITH_FEATURES__TRAITS_HPP_
#define SVO_MSGS__MSG__DETAIL__DENSE_INPUT_WITH_FEATURES__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "svo_msgs/msg/detail/dense_input_with_features__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

// Include directives for member types
// Member 'header'
#include "std_msgs/msg/detail/header__traits.hpp"
// Member 'pose'
#include "geometry_msgs/msg/detail/pose__traits.hpp"
// Member 'image'
#include "sensor_msgs/msg/detail/image__traits.hpp"
// Member 'features'
#include "svo_msgs/msg/detail/feature__traits.hpp"

namespace svo_msgs
{

namespace msg
{

inline void to_flow_style_yaml(
  const DenseInputWithFeatures & msg,
  std::ostream & out)
{
  out << "{";
  // member: header
  {
    out << "header: ";
    to_flow_style_yaml(msg.header, out);
    out << ", ";
  }

  // member: frame_id
  {
    out << "frame_id: ";
    rosidl_generator_traits::value_to_yaml(msg.frame_id, out);
    out << ", ";
  }

  // member: pose
  {
    out << "pose: ";
    to_flow_style_yaml(msg.pose, out);
    out << ", ";
  }

  // member: image
  {
    out << "image: ";
    to_flow_style_yaml(msg.image, out);
    out << ", ";
  }

  // member: min_depth
  {
    out << "min_depth: ";
    rosidl_generator_traits::value_to_yaml(msg.min_depth, out);
    out << ", ";
  }

  // member: max_depth
  {
    out << "max_depth: ";
    rosidl_generator_traits::value_to_yaml(msg.max_depth, out);
    out << ", ";
  }

  // member: features
  {
    if (msg.features.size() == 0) {
      out << "features: []";
    } else {
      out << "features: [";
      size_t pending_items = msg.features.size();
      for (auto item : msg.features) {
        to_flow_style_yaml(item, out);
        if (--pending_items > 0) {
          out << ", ";
        }
      }
      out << "]";
    }
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const DenseInputWithFeatures & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: header
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "header:\n";
    to_block_style_yaml(msg.header, out, indentation + 2);
  }

  // member: frame_id
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "frame_id: ";
    rosidl_generator_traits::value_to_yaml(msg.frame_id, out);
    out << "\n";
  }

  // member: pose
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "pose:\n";
    to_block_style_yaml(msg.pose, out, indentation + 2);
  }

  // member: image
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "image:\n";
    to_block_style_yaml(msg.image, out, indentation + 2);
  }

  // member: min_depth
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "min_depth: ";
    rosidl_generator_traits::value_to_yaml(msg.min_depth, out);
    out << "\n";
  }

  // member: max_depth
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "max_depth: ";
    rosidl_generator_traits::value_to_yaml(msg.max_depth, out);
    out << "\n";
  }

  // member: features
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    if (msg.features.size() == 0) {
      out << "features: []\n";
    } else {
      out << "features:\n";
      for (auto item : msg.features) {
        if (indentation > 0) {
          out << std::string(indentation, ' ');
        }
        out << "-\n";
        to_block_style_yaml(item, out, indentation + 2);
      }
    }
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const DenseInputWithFeatures & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace msg

}  // namespace svo_msgs

namespace rosidl_generator_traits
{

[[deprecated("use svo_msgs::msg::to_block_style_yaml() instead")]]
inline void to_yaml(
  const svo_msgs::msg::DenseInputWithFeatures & msg,
  std::ostream & out, size_t indentation = 0)
{
  svo_msgs::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use svo_msgs::msg::to_yaml() instead")]]
inline std::string to_yaml(const svo_msgs::msg::DenseInputWithFeatures & msg)
{
  return svo_msgs::msg::to_yaml(msg);
}

template<>
inline const char * data_type<svo_msgs::msg::DenseInputWithFeatures>()
{
  return "svo_msgs::msg::DenseInputWithFeatures";
}

template<>
inline const char * name<svo_msgs::msg::DenseInputWithFeatures>()
{
  return "svo_msgs/msg/DenseInputWithFeatures";
}

template<>
struct has_fixed_size<svo_msgs::msg::DenseInputWithFeatures>
  : std::integral_constant<bool, false> {};

template<>
struct has_bounded_size<svo_msgs::msg::DenseInputWithFeatures>
  : std::integral_constant<bool, false> {};

template<>
struct is_message<svo_msgs::msg::DenseInputWithFeatures>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // SVO_MSGS__MSG__DETAIL__DENSE_INPUT_WITH_FEATURES__TRAITS_HPP_
