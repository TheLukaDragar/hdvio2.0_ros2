// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from svo_msgs:msg/Feature.idl
// generated code does not contain a copyright notice

#ifndef SVO_MSGS__MSG__DETAIL__FEATURE__BUILDER_HPP_
#define SVO_MSGS__MSG__DETAIL__FEATURE__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "svo_msgs/msg/detail/feature__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace svo_msgs
{

namespace msg
{

namespace builder
{

class Init_Feature_z
{
public:
  explicit Init_Feature_z(::svo_msgs::msg::Feature & msg)
  : msg_(msg)
  {}
  ::svo_msgs::msg::Feature z(::svo_msgs::msg::Feature::_z_type arg)
  {
    msg_.z = std::move(arg);
    return std::move(msg_);
  }

private:
  ::svo_msgs::msg::Feature msg_;
};

class Init_Feature_y
{
public:
  explicit Init_Feature_y(::svo_msgs::msg::Feature & msg)
  : msg_(msg)
  {}
  Init_Feature_z y(::svo_msgs::msg::Feature::_y_type arg)
  {
    msg_.y = std::move(arg);
    return Init_Feature_z(msg_);
  }

private:
  ::svo_msgs::msg::Feature msg_;
};

class Init_Feature_x
{
public:
  Init_Feature_x()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_Feature_y x(::svo_msgs::msg::Feature::_x_type arg)
  {
    msg_.x = std::move(arg);
    return Init_Feature_y(msg_);
  }

private:
  ::svo_msgs::msg::Feature msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::svo_msgs::msg::Feature>()
{
  return svo_msgs::msg::builder::Init_Feature_x();
}

}  // namespace svo_msgs

#endif  // SVO_MSGS__MSG__DETAIL__FEATURE__BUILDER_HPP_
