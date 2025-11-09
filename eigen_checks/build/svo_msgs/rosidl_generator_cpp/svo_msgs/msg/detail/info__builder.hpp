// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from svo_msgs:msg/Info.idl
// generated code does not contain a copyright notice

#ifndef SVO_MSGS__MSG__DETAIL__INFO__BUILDER_HPP_
#define SVO_MSGS__MSG__DETAIL__INFO__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "svo_msgs/msg/detail/info__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace svo_msgs
{

namespace msg
{

namespace builder
{

class Init_Info_stage
{
public:
  explicit Init_Info_stage(::svo_msgs::msg::Info & msg)
  : msg_(msg)
  {}
  ::svo_msgs::msg::Info stage(::svo_msgs::msg::Info::_stage_type arg)
  {
    msg_.stage = std::move(arg);
    return std::move(msg_);
  }

private:
  ::svo_msgs::msg::Info msg_;
};

class Init_Info_tracking_quality
{
public:
  explicit Init_Info_tracking_quality(::svo_msgs::msg::Info & msg)
  : msg_(msg)
  {}
  Init_Info_stage tracking_quality(::svo_msgs::msg::Info::_tracking_quality_type arg)
  {
    msg_.tracking_quality = std::move(arg);
    return Init_Info_stage(msg_);
  }

private:
  ::svo_msgs::msg::Info msg_;
};

class Init_Info_num_matches
{
public:
  explicit Init_Info_num_matches(::svo_msgs::msg::Info & msg)
  : msg_(msg)
  {}
  Init_Info_tracking_quality num_matches(::svo_msgs::msg::Info::_num_matches_type arg)
  {
    msg_.num_matches = std::move(arg);
    return Init_Info_tracking_quality(msg_);
  }

private:
  ::svo_msgs::msg::Info msg_;
};

class Init_Info_keyframes
{
public:
  explicit Init_Info_keyframes(::svo_msgs::msg::Info & msg)
  : msg_(msg)
  {}
  Init_Info_num_matches keyframes(::svo_msgs::msg::Info::_keyframes_type arg)
  {
    msg_.keyframes = std::move(arg);
    return Init_Info_num_matches(msg_);
  }

private:
  ::svo_msgs::msg::Info msg_;
};

class Init_Info_processing_time
{
public:
  explicit Init_Info_processing_time(::svo_msgs::msg::Info & msg)
  : msg_(msg)
  {}
  Init_Info_keyframes processing_time(::svo_msgs::msg::Info::_processing_time_type arg)
  {
    msg_.processing_time = std::move(arg);
    return Init_Info_keyframes(msg_);
  }

private:
  ::svo_msgs::msg::Info msg_;
};

class Init_Info_header
{
public:
  Init_Info_header()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_Info_processing_time header(::svo_msgs::msg::Info::_header_type arg)
  {
    msg_.header = std::move(arg);
    return Init_Info_processing_time(msg_);
  }

private:
  ::svo_msgs::msg::Info msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::svo_msgs::msg::Info>()
{
  return svo_msgs::msg::builder::Init_Info_header();
}

}  // namespace svo_msgs

#endif  // SVO_MSGS__MSG__DETAIL__INFO__BUILDER_HPP_
