// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from svo_msgs:msg/DenseInputWithFeatures.idl
// generated code does not contain a copyright notice

#ifndef SVO_MSGS__MSG__DETAIL__DENSE_INPUT_WITH_FEATURES__BUILDER_HPP_
#define SVO_MSGS__MSG__DETAIL__DENSE_INPUT_WITH_FEATURES__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "svo_msgs/msg/detail/dense_input_with_features__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace svo_msgs
{

namespace msg
{

namespace builder
{

class Init_DenseInputWithFeatures_features
{
public:
  explicit Init_DenseInputWithFeatures_features(::svo_msgs::msg::DenseInputWithFeatures & msg)
  : msg_(msg)
  {}
  ::svo_msgs::msg::DenseInputWithFeatures features(::svo_msgs::msg::DenseInputWithFeatures::_features_type arg)
  {
    msg_.features = std::move(arg);
    return std::move(msg_);
  }

private:
  ::svo_msgs::msg::DenseInputWithFeatures msg_;
};

class Init_DenseInputWithFeatures_max_depth
{
public:
  explicit Init_DenseInputWithFeatures_max_depth(::svo_msgs::msg::DenseInputWithFeatures & msg)
  : msg_(msg)
  {}
  Init_DenseInputWithFeatures_features max_depth(::svo_msgs::msg::DenseInputWithFeatures::_max_depth_type arg)
  {
    msg_.max_depth = std::move(arg);
    return Init_DenseInputWithFeatures_features(msg_);
  }

private:
  ::svo_msgs::msg::DenseInputWithFeatures msg_;
};

class Init_DenseInputWithFeatures_min_depth
{
public:
  explicit Init_DenseInputWithFeatures_min_depth(::svo_msgs::msg::DenseInputWithFeatures & msg)
  : msg_(msg)
  {}
  Init_DenseInputWithFeatures_max_depth min_depth(::svo_msgs::msg::DenseInputWithFeatures::_min_depth_type arg)
  {
    msg_.min_depth = std::move(arg);
    return Init_DenseInputWithFeatures_max_depth(msg_);
  }

private:
  ::svo_msgs::msg::DenseInputWithFeatures msg_;
};

class Init_DenseInputWithFeatures_image
{
public:
  explicit Init_DenseInputWithFeatures_image(::svo_msgs::msg::DenseInputWithFeatures & msg)
  : msg_(msg)
  {}
  Init_DenseInputWithFeatures_min_depth image(::svo_msgs::msg::DenseInputWithFeatures::_image_type arg)
  {
    msg_.image = std::move(arg);
    return Init_DenseInputWithFeatures_min_depth(msg_);
  }

private:
  ::svo_msgs::msg::DenseInputWithFeatures msg_;
};

class Init_DenseInputWithFeatures_pose
{
public:
  explicit Init_DenseInputWithFeatures_pose(::svo_msgs::msg::DenseInputWithFeatures & msg)
  : msg_(msg)
  {}
  Init_DenseInputWithFeatures_image pose(::svo_msgs::msg::DenseInputWithFeatures::_pose_type arg)
  {
    msg_.pose = std::move(arg);
    return Init_DenseInputWithFeatures_image(msg_);
  }

private:
  ::svo_msgs::msg::DenseInputWithFeatures msg_;
};

class Init_DenseInputWithFeatures_frame_id
{
public:
  explicit Init_DenseInputWithFeatures_frame_id(::svo_msgs::msg::DenseInputWithFeatures & msg)
  : msg_(msg)
  {}
  Init_DenseInputWithFeatures_pose frame_id(::svo_msgs::msg::DenseInputWithFeatures::_frame_id_type arg)
  {
    msg_.frame_id = std::move(arg);
    return Init_DenseInputWithFeatures_pose(msg_);
  }

private:
  ::svo_msgs::msg::DenseInputWithFeatures msg_;
};

class Init_DenseInputWithFeatures_header
{
public:
  Init_DenseInputWithFeatures_header()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_DenseInputWithFeatures_frame_id header(::svo_msgs::msg::DenseInputWithFeatures::_header_type arg)
  {
    msg_.header = std::move(arg);
    return Init_DenseInputWithFeatures_frame_id(msg_);
  }

private:
  ::svo_msgs::msg::DenseInputWithFeatures msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::svo_msgs::msg::DenseInputWithFeatures>()
{
  return svo_msgs::msg::builder::Init_DenseInputWithFeatures_header();
}

}  // namespace svo_msgs

#endif  // SVO_MSGS__MSG__DETAIL__DENSE_INPUT_WITH_FEATURES__BUILDER_HPP_
