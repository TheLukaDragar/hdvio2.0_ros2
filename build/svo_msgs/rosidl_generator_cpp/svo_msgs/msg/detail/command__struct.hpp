// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from svo_msgs:msg/Command.idl
// generated code does not contain a copyright notice

#ifndef SVO_MSGS__MSG__DETAIL__COMMAND__STRUCT_HPP_
#define SVO_MSGS__MSG__DETAIL__COMMAND__STRUCT_HPP_

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
// Member 'bodyrates'
#include "geometry_msgs/msg/detail/vector3__struct.hpp"

#ifndef _WIN32
# define DEPRECATED__svo_msgs__msg__Command __attribute__((deprecated))
#else
# define DEPRECATED__svo_msgs__msg__Command __declspec(deprecated)
#endif

namespace svo_msgs
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct Command_
{
  using Type = Command_<ContainerAllocator>;

  explicit Command_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : header(_init),
    bodyrates(_init)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->t = 0.0;
      this->is_single_rotor_thrust = false;
      this->collective_thrust = 0.0;
      std::fill<typename std::array<double, 4>::iterator, double>(this->thrusts.begin(), this->thrusts.end(), 0.0);
    }
  }

  explicit Command_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : header(_alloc, _init),
    bodyrates(_alloc, _init),
    thrusts(_alloc)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->t = 0.0;
      this->is_single_rotor_thrust = false;
      this->collective_thrust = 0.0;
      std::fill<typename std::array<double, 4>::iterator, double>(this->thrusts.begin(), this->thrusts.end(), 0.0);
    }
  }

  // field types and members
  using _header_type =
    std_msgs::msg::Header_<ContainerAllocator>;
  _header_type header;
  using _t_type =
    double;
  _t_type t;
  using _is_single_rotor_thrust_type =
    bool;
  _is_single_rotor_thrust_type is_single_rotor_thrust;
  using _collective_thrust_type =
    double;
  _collective_thrust_type collective_thrust;
  using _bodyrates_type =
    geometry_msgs::msg::Vector3_<ContainerAllocator>;
  _bodyrates_type bodyrates;
  using _thrusts_type =
    std::array<double, 4>;
  _thrusts_type thrusts;

  // setters for named parameter idiom
  Type & set__header(
    const std_msgs::msg::Header_<ContainerAllocator> & _arg)
  {
    this->header = _arg;
    return *this;
  }
  Type & set__t(
    const double & _arg)
  {
    this->t = _arg;
    return *this;
  }
  Type & set__is_single_rotor_thrust(
    const bool & _arg)
  {
    this->is_single_rotor_thrust = _arg;
    return *this;
  }
  Type & set__collective_thrust(
    const double & _arg)
  {
    this->collective_thrust = _arg;
    return *this;
  }
  Type & set__bodyrates(
    const geometry_msgs::msg::Vector3_<ContainerAllocator> & _arg)
  {
    this->bodyrates = _arg;
    return *this;
  }
  Type & set__thrusts(
    const std::array<double, 4> & _arg)
  {
    this->thrusts = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    svo_msgs::msg::Command_<ContainerAllocator> *;
  using ConstRawPtr =
    const svo_msgs::msg::Command_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<svo_msgs::msg::Command_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<svo_msgs::msg::Command_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      svo_msgs::msg::Command_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<svo_msgs::msg::Command_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      svo_msgs::msg::Command_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<svo_msgs::msg::Command_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<svo_msgs::msg::Command_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<svo_msgs::msg::Command_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__svo_msgs__msg__Command
    std::shared_ptr<svo_msgs::msg::Command_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__svo_msgs__msg__Command
    std::shared_ptr<svo_msgs::msg::Command_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const Command_ & other) const
  {
    if (this->header != other.header) {
      return false;
    }
    if (this->t != other.t) {
      return false;
    }
    if (this->is_single_rotor_thrust != other.is_single_rotor_thrust) {
      return false;
    }
    if (this->collective_thrust != other.collective_thrust) {
      return false;
    }
    if (this->bodyrates != other.bodyrates) {
      return false;
    }
    if (this->thrusts != other.thrusts) {
      return false;
    }
    return true;
  }
  bool operator!=(const Command_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct Command_

// alias to use template instance with default allocator
using Command =
  svo_msgs::msg::Command_<std::allocator<void>>;

// constant definitions

}  // namespace msg

}  // namespace svo_msgs

#endif  // SVO_MSGS__MSG__DETAIL__COMMAND__STRUCT_HPP_
