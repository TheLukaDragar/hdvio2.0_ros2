// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from svo_msgs:msg/Info.idl
// generated code does not contain a copyright notice

#ifndef SVO_MSGS__MSG__DETAIL__INFO__STRUCT_HPP_
#define SVO_MSGS__MSG__DETAIL__INFO__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


// Include directives for member types
// Member 'header'
#include "std_msgs/msg/detail/header__struct.hpp"

#ifndef _WIN32
# define DEPRECATED__svo_msgs__msg__Info __attribute__((deprecated))
#else
# define DEPRECATED__svo_msgs__msg__Info __declspec(deprecated)
#endif

namespace svo_msgs
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct Info_
{
  using Type = Info_<ContainerAllocator>;

  explicit Info_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : header(_init)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->processing_time = 0.0f;
      this->num_matches = 0l;
      this->tracking_quality = 0l;
      this->stage = 0l;
    }
  }

  explicit Info_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : header(_alloc, _init)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->processing_time = 0.0f;
      this->num_matches = 0l;
      this->tracking_quality = 0l;
      this->stage = 0l;
    }
  }

  // field types and members
  using _header_type =
    std_msgs::msg::Header_<ContainerAllocator>;
  _header_type header;
  using _processing_time_type =
    float;
  _processing_time_type processing_time;
  using _keyframes_type =
    std::vector<int32_t, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<int32_t>>;
  _keyframes_type keyframes;
  using _num_matches_type =
    int32_t;
  _num_matches_type num_matches;
  using _tracking_quality_type =
    int32_t;
  _tracking_quality_type tracking_quality;
  using _stage_type =
    int32_t;
  _stage_type stage;

  // setters for named parameter idiom
  Type & set__header(
    const std_msgs::msg::Header_<ContainerAllocator> & _arg)
  {
    this->header = _arg;
    return *this;
  }
  Type & set__processing_time(
    const float & _arg)
  {
    this->processing_time = _arg;
    return *this;
  }
  Type & set__keyframes(
    const std::vector<int32_t, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<int32_t>> & _arg)
  {
    this->keyframes = _arg;
    return *this;
  }
  Type & set__num_matches(
    const int32_t & _arg)
  {
    this->num_matches = _arg;
    return *this;
  }
  Type & set__tracking_quality(
    const int32_t & _arg)
  {
    this->tracking_quality = _arg;
    return *this;
  }
  Type & set__stage(
    const int32_t & _arg)
  {
    this->stage = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    svo_msgs::msg::Info_<ContainerAllocator> *;
  using ConstRawPtr =
    const svo_msgs::msg::Info_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<svo_msgs::msg::Info_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<svo_msgs::msg::Info_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      svo_msgs::msg::Info_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<svo_msgs::msg::Info_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      svo_msgs::msg::Info_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<svo_msgs::msg::Info_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<svo_msgs::msg::Info_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<svo_msgs::msg::Info_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__svo_msgs__msg__Info
    std::shared_ptr<svo_msgs::msg::Info_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__svo_msgs__msg__Info
    std::shared_ptr<svo_msgs::msg::Info_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const Info_ & other) const
  {
    if (this->header != other.header) {
      return false;
    }
    if (this->processing_time != other.processing_time) {
      return false;
    }
    if (this->keyframes != other.keyframes) {
      return false;
    }
    if (this->num_matches != other.num_matches) {
      return false;
    }
    if (this->tracking_quality != other.tracking_quality) {
      return false;
    }
    if (this->stage != other.stage) {
      return false;
    }
    return true;
  }
  bool operator!=(const Info_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct Info_

// alias to use template instance with default allocator
using Info =
  svo_msgs::msg::Info_<std::allocator<void>>;

// constant definitions

}  // namespace msg

}  // namespace svo_msgs

#endif  // SVO_MSGS__MSG__DETAIL__INFO__STRUCT_HPP_
