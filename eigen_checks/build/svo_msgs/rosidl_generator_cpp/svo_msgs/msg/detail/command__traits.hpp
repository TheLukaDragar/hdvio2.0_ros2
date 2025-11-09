// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from svo_msgs:msg/Command.idl
// generated code does not contain a copyright notice

#ifndef SVO_MSGS__MSG__DETAIL__COMMAND__TRAITS_HPP_
#define SVO_MSGS__MSG__DETAIL__COMMAND__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "svo_msgs/msg/detail/command__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

// Include directives for member types
// Member 'header'
#include "std_msgs/msg/detail/header__traits.hpp"
// Member 'bodyrates'
#include "geometry_msgs/msg/detail/vector3__traits.hpp"

namespace svo_msgs
{

namespace msg
{

inline void to_flow_style_yaml(
  const Command & msg,
  std::ostream & out)
{
  out << "{";
  // member: header
  {
    out << "header: ";
    to_flow_style_yaml(msg.header, out);
    out << ", ";
  }

  // member: t
  {
    out << "t: ";
    rosidl_generator_traits::value_to_yaml(msg.t, out);
    out << ", ";
  }

  // member: is_single_rotor_thrust
  {
    out << "is_single_rotor_thrust: ";
    rosidl_generator_traits::value_to_yaml(msg.is_single_rotor_thrust, out);
    out << ", ";
  }

  // member: collective_thrust
  {
    out << "collective_thrust: ";
    rosidl_generator_traits::value_to_yaml(msg.collective_thrust, out);
    out << ", ";
  }

  // member: bodyrates
  {
    out << "bodyrates: ";
    to_flow_style_yaml(msg.bodyrates, out);
    out << ", ";
  }

  // member: thrusts
  {
    if (msg.thrusts.size() == 0) {
      out << "thrusts: []";
    } else {
      out << "thrusts: [";
      size_t pending_items = msg.thrusts.size();
      for (auto item : msg.thrusts) {
        rosidl_generator_traits::value_to_yaml(item, out);
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
  const Command & msg,
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

  // member: t
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "t: ";
    rosidl_generator_traits::value_to_yaml(msg.t, out);
    out << "\n";
  }

  // member: is_single_rotor_thrust
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "is_single_rotor_thrust: ";
    rosidl_generator_traits::value_to_yaml(msg.is_single_rotor_thrust, out);
    out << "\n";
  }

  // member: collective_thrust
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "collective_thrust: ";
    rosidl_generator_traits::value_to_yaml(msg.collective_thrust, out);
    out << "\n";
  }

  // member: bodyrates
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "bodyrates:\n";
    to_block_style_yaml(msg.bodyrates, out, indentation + 2);
  }

  // member: thrusts
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    if (msg.thrusts.size() == 0) {
      out << "thrusts: []\n";
    } else {
      out << "thrusts:\n";
      for (auto item : msg.thrusts) {
        if (indentation > 0) {
          out << std::string(indentation, ' ');
        }
        out << "- ";
        rosidl_generator_traits::value_to_yaml(item, out);
        out << "\n";
      }
    }
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const Command & msg, bool use_flow_style = false)
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
  const svo_msgs::msg::Command & msg,
  std::ostream & out, size_t indentation = 0)
{
  svo_msgs::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use svo_msgs::msg::to_yaml() instead")]]
inline std::string to_yaml(const svo_msgs::msg::Command & msg)
{
  return svo_msgs::msg::to_yaml(msg);
}

template<>
inline const char * data_type<svo_msgs::msg::Command>()
{
  return "svo_msgs::msg::Command";
}

template<>
inline const char * name<svo_msgs::msg::Command>()
{
  return "svo_msgs/msg/Command";
}

template<>
struct has_fixed_size<svo_msgs::msg::Command>
  : std::integral_constant<bool, has_fixed_size<geometry_msgs::msg::Vector3>::value && has_fixed_size<std_msgs::msg::Header>::value> {};

template<>
struct has_bounded_size<svo_msgs::msg::Command>
  : std::integral_constant<bool, has_bounded_size<geometry_msgs::msg::Vector3>::value && has_bounded_size<std_msgs::msg::Header>::value> {};

template<>
struct is_message<svo_msgs::msg::Command>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // SVO_MSGS__MSG__DETAIL__COMMAND__TRAITS_HPP_
