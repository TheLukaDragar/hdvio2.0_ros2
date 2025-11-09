// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from svo_msgs:msg/Command.idl
// generated code does not contain a copyright notice

#ifndef SVO_MSGS__MSG__DETAIL__COMMAND__BUILDER_HPP_
#define SVO_MSGS__MSG__DETAIL__COMMAND__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "svo_msgs/msg/detail/command__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace svo_msgs
{

namespace msg
{

namespace builder
{

class Init_Command_thrusts
{
public:
  explicit Init_Command_thrusts(::svo_msgs::msg::Command & msg)
  : msg_(msg)
  {}
  ::svo_msgs::msg::Command thrusts(::svo_msgs::msg::Command::_thrusts_type arg)
  {
    msg_.thrusts = std::move(arg);
    return std::move(msg_);
  }

private:
  ::svo_msgs::msg::Command msg_;
};

class Init_Command_bodyrates
{
public:
  explicit Init_Command_bodyrates(::svo_msgs::msg::Command & msg)
  : msg_(msg)
  {}
  Init_Command_thrusts bodyrates(::svo_msgs::msg::Command::_bodyrates_type arg)
  {
    msg_.bodyrates = std::move(arg);
    return Init_Command_thrusts(msg_);
  }

private:
  ::svo_msgs::msg::Command msg_;
};

class Init_Command_collective_thrust
{
public:
  explicit Init_Command_collective_thrust(::svo_msgs::msg::Command & msg)
  : msg_(msg)
  {}
  Init_Command_bodyrates collective_thrust(::svo_msgs::msg::Command::_collective_thrust_type arg)
  {
    msg_.collective_thrust = std::move(arg);
    return Init_Command_bodyrates(msg_);
  }

private:
  ::svo_msgs::msg::Command msg_;
};

class Init_Command_is_single_rotor_thrust
{
public:
  explicit Init_Command_is_single_rotor_thrust(::svo_msgs::msg::Command & msg)
  : msg_(msg)
  {}
  Init_Command_collective_thrust is_single_rotor_thrust(::svo_msgs::msg::Command::_is_single_rotor_thrust_type arg)
  {
    msg_.is_single_rotor_thrust = std::move(arg);
    return Init_Command_collective_thrust(msg_);
  }

private:
  ::svo_msgs::msg::Command msg_;
};

class Init_Command_t
{
public:
  explicit Init_Command_t(::svo_msgs::msg::Command & msg)
  : msg_(msg)
  {}
  Init_Command_is_single_rotor_thrust t(::svo_msgs::msg::Command::_t_type arg)
  {
    msg_.t = std::move(arg);
    return Init_Command_is_single_rotor_thrust(msg_);
  }

private:
  ::svo_msgs::msg::Command msg_;
};

class Init_Command_header
{
public:
  Init_Command_header()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_Command_t header(::svo_msgs::msg::Command::_header_type arg)
  {
    msg_.header = std::move(arg);
    return Init_Command_t(msg_);
  }

private:
  ::svo_msgs::msg::Command msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::svo_msgs::msg::Command>()
{
  return svo_msgs::msg::builder::Init_Command_header();
}

}  // namespace svo_msgs

#endif  // SVO_MSGS__MSG__DETAIL__COMMAND__BUILDER_HPP_
