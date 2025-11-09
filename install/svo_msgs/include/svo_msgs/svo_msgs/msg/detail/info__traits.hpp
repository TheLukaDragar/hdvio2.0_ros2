// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from svo_msgs:msg/Info.idl
// generated code does not contain a copyright notice

#ifndef SVO_MSGS__MSG__DETAIL__INFO__TRAITS_HPP_
#define SVO_MSGS__MSG__DETAIL__INFO__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "svo_msgs/msg/detail/info__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

// Include directives for member types
// Member 'header'
#include "std_msgs/msg/detail/header__traits.hpp"

namespace svo_msgs
{

namespace msg
{

inline void to_flow_style_yaml(
  const Info & msg,
  std::ostream & out)
{
  out << "{";
  // member: header
  {
    out << "header: ";
    to_flow_style_yaml(msg.header, out);
    out << ", ";
  }

  // member: processing_time
  {
    out << "processing_time: ";
    rosidl_generator_traits::value_to_yaml(msg.processing_time, out);
    out << ", ";
  }

  // member: keyframes
  {
    if (msg.keyframes.size() == 0) {
      out << "keyframes: []";
    } else {
      out << "keyframes: [";
      size_t pending_items = msg.keyframes.size();
      for (auto item : msg.keyframes) {
        rosidl_generator_traits::value_to_yaml(item, out);
        if (--pending_items > 0) {
          out << ", ";
        }
      }
      out << "]";
    }
    out << ", ";
  }

  // member: num_matches
  {
    out << "num_matches: ";
    rosidl_generator_traits::value_to_yaml(msg.num_matches, out);
    out << ", ";
  }

  // member: tracking_quality
  {
    out << "tracking_quality: ";
    rosidl_generator_traits::value_to_yaml(msg.tracking_quality, out);
    out << ", ";
  }

  // member: stage
  {
    out << "stage: ";
    rosidl_generator_traits::value_to_yaml(msg.stage, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const Info & msg,
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

  // member: processing_time
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "processing_time: ";
    rosidl_generator_traits::value_to_yaml(msg.processing_time, out);
    out << "\n";
  }

  // member: keyframes
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    if (msg.keyframes.size() == 0) {
      out << "keyframes: []\n";
    } else {
      out << "keyframes:\n";
      for (auto item : msg.keyframes) {
        if (indentation > 0) {
          out << std::string(indentation, ' ');
        }
        out << "- ";
        rosidl_generator_traits::value_to_yaml(item, out);
        out << "\n";
      }
    }
  }

  // member: num_matches
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "num_matches: ";
    rosidl_generator_traits::value_to_yaml(msg.num_matches, out);
    out << "\n";
  }

  // member: tracking_quality
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "tracking_quality: ";
    rosidl_generator_traits::value_to_yaml(msg.tracking_quality, out);
    out << "\n";
  }

  // member: stage
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "stage: ";
    rosidl_generator_traits::value_to_yaml(msg.stage, out);
    out << "\n";
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const Info & msg, bool use_flow_style = false)
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
  const svo_msgs::msg::Info & msg,
  std::ostream & out, size_t indentation = 0)
{
  svo_msgs::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use svo_msgs::msg::to_yaml() instead")]]
inline std::string to_yaml(const svo_msgs::msg::Info & msg)
{
  return svo_msgs::msg::to_yaml(msg);
}

template<>
inline const char * data_type<svo_msgs::msg::Info>()
{
  return "svo_msgs::msg::Info";
}

template<>
inline const char * name<svo_msgs::msg::Info>()
{
  return "svo_msgs/msg/Info";
}

template<>
struct has_fixed_size<svo_msgs::msg::Info>
  : std::integral_constant<bool, false> {};

template<>
struct has_bounded_size<svo_msgs::msg::Info>
  : std::integral_constant<bool, false> {};

template<>
struct is_message<svo_msgs::msg::Info>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // SVO_MSGS__MSG__DETAIL__INFO__TRAITS_HPP_
