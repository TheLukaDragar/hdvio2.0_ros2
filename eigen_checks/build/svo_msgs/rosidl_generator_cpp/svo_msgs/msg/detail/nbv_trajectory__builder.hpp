// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from svo_msgs:msg/NbvTrajectory.idl
// generated code does not contain a copyright notice

#ifndef SVO_MSGS__MSG__DETAIL__NBV_TRAJECTORY__BUILDER_HPP_
#define SVO_MSGS__MSG__DETAIL__NBV_TRAJECTORY__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "svo_msgs/msg/detail/nbv_trajectory__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace svo_msgs
{

namespace msg
{

namespace builder
{

class Init_NbvTrajectory_trajectory
{
public:
  explicit Init_NbvTrajectory_trajectory(::svo_msgs::msg::NbvTrajectory & msg)
  : msg_(msg)
  {}
  ::svo_msgs::msg::NbvTrajectory trajectory(::svo_msgs::msg::NbvTrajectory::_trajectory_type arg)
  {
    msg_.trajectory = std::move(arg);
    return std::move(msg_);
  }

private:
  ::svo_msgs::msg::NbvTrajectory msg_;
};

class Init_NbvTrajectory_header
{
public:
  Init_NbvTrajectory_header()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_NbvTrajectory_trajectory header(::svo_msgs::msg::NbvTrajectory::_header_type arg)
  {
    msg_.header = std::move(arg);
    return Init_NbvTrajectory_trajectory(msg_);
  }

private:
  ::svo_msgs::msg::NbvTrajectory msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::svo_msgs::msg::NbvTrajectory>()
{
  return svo_msgs::msg::builder::Init_NbvTrajectory_header();
}

}  // namespace svo_msgs

#endif  // SVO_MSGS__MSG__DETAIL__NBV_TRAJECTORY__BUILDER_HPP_
