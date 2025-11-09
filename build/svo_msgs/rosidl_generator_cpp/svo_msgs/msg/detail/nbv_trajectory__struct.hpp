// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from svo_msgs:msg/NbvTrajectory.idl
// generated code does not contain a copyright notice

#ifndef SVO_MSGS__MSG__DETAIL__NBV_TRAJECTORY__STRUCT_HPP_
#define SVO_MSGS__MSG__DETAIL__NBV_TRAJECTORY__STRUCT_HPP_

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
// Member 'trajectory'
#include "geometry_msgs/msg/detail/pose__struct.hpp"

#ifndef _WIN32
# define DEPRECATED__svo_msgs__msg__NbvTrajectory __attribute__((deprecated))
#else
# define DEPRECATED__svo_msgs__msg__NbvTrajectory __declspec(deprecated)
#endif

namespace svo_msgs
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct NbvTrajectory_
{
  using Type = NbvTrajectory_<ContainerAllocator>;

  explicit NbvTrajectory_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : header(_init)
  {
    (void)_init;
  }

  explicit NbvTrajectory_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : header(_alloc, _init)
  {
    (void)_init;
  }

  // field types and members
  using _header_type =
    std_msgs::msg::Header_<ContainerAllocator>;
  _header_type header;
  using _trajectory_type =
    std::vector<geometry_msgs::msg::Pose_<ContainerAllocator>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<geometry_msgs::msg::Pose_<ContainerAllocator>>>;
  _trajectory_type trajectory;

  // setters for named parameter idiom
  Type & set__header(
    const std_msgs::msg::Header_<ContainerAllocator> & _arg)
  {
    this->header = _arg;
    return *this;
  }
  Type & set__trajectory(
    const std::vector<geometry_msgs::msg::Pose_<ContainerAllocator>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<geometry_msgs::msg::Pose_<ContainerAllocator>>> & _arg)
  {
    this->trajectory = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    svo_msgs::msg::NbvTrajectory_<ContainerAllocator> *;
  using ConstRawPtr =
    const svo_msgs::msg::NbvTrajectory_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<svo_msgs::msg::NbvTrajectory_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<svo_msgs::msg::NbvTrajectory_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      svo_msgs::msg::NbvTrajectory_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<svo_msgs::msg::NbvTrajectory_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      svo_msgs::msg::NbvTrajectory_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<svo_msgs::msg::NbvTrajectory_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<svo_msgs::msg::NbvTrajectory_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<svo_msgs::msg::NbvTrajectory_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__svo_msgs__msg__NbvTrajectory
    std::shared_ptr<svo_msgs::msg::NbvTrajectory_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__svo_msgs__msg__NbvTrajectory
    std::shared_ptr<svo_msgs::msg::NbvTrajectory_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const NbvTrajectory_ & other) const
  {
    if (this->header != other.header) {
      return false;
    }
    if (this->trajectory != other.trajectory) {
      return false;
    }
    return true;
  }
  bool operator!=(const NbvTrajectory_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct NbvTrajectory_

// alias to use template instance with default allocator
using NbvTrajectory =
  svo_msgs::msg::NbvTrajectory_<std::allocator<void>>;

// constant definitions

}  // namespace msg

}  // namespace svo_msgs

#endif  // SVO_MSGS__MSG__DETAIL__NBV_TRAJECTORY__STRUCT_HPP_
