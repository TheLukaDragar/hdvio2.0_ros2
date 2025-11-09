// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from svo_msgs:msg/Feature.idl
// generated code does not contain a copyright notice

#ifndef SVO_MSGS__MSG__DETAIL__FEATURE__TRAITS_HPP_
#define SVO_MSGS__MSG__DETAIL__FEATURE__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "svo_msgs/msg/detail/feature__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

namespace svo_msgs
{

namespace msg
{

inline void to_flow_style_yaml(
  const Feature & msg,
  std::ostream & out)
{
  out << "{";
  // member: x
  {
    out << "x: ";
    rosidl_generator_traits::value_to_yaml(msg.x, out);
    out << ", ";
  }

  // member: y
  {
    out << "y: ";
    rosidl_generator_traits::value_to_yaml(msg.y, out);
    out << ", ";
  }

  // member: z
  {
    out << "z: ";
    rosidl_generator_traits::value_to_yaml(msg.z, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const Feature & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: x
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "x: ";
    rosidl_generator_traits::value_to_yaml(msg.x, out);
    out << "\n";
  }

  // member: y
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "y: ";
    rosidl_generator_traits::value_to_yaml(msg.y, out);
    out << "\n";
  }

  // member: z
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "z: ";
    rosidl_generator_traits::value_to_yaml(msg.z, out);
    out << "\n";
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const Feature & msg, bool use_flow_style = false)
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
  const svo_msgs::msg::Feature & msg,
  std::ostream & out, size_t indentation = 0)
{
  svo_msgs::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use svo_msgs::msg::to_yaml() instead")]]
inline std::string to_yaml(const svo_msgs::msg::Feature & msg)
{
  return svo_msgs::msg::to_yaml(msg);
}

template<>
inline const char * data_type<svo_msgs::msg::Feature>()
{
  return "svo_msgs::msg::Feature";
}

template<>
inline const char * name<svo_msgs::msg::Feature>()
{
  return "svo_msgs/msg/Feature";
}

template<>
struct has_fixed_size<svo_msgs::msg::Feature>
  : std::integral_constant<bool, true> {};

template<>
struct has_bounded_size<svo_msgs::msg::Feature>
  : std::integral_constant<bool, true> {};

template<>
struct is_message<svo_msgs::msg::Feature>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // SVO_MSGS__MSG__DETAIL__FEATURE__TRAITS_HPP_
