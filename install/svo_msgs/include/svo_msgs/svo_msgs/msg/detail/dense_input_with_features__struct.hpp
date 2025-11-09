// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from svo_msgs:msg/DenseInputWithFeatures.idl
// generated code does not contain a copyright notice

#ifndef SVO_MSGS__MSG__DETAIL__DENSE_INPUT_WITH_FEATURES__STRUCT_HPP_
#define SVO_MSGS__MSG__DETAIL__DENSE_INPUT_WITH_FEATURES__STRUCT_HPP_

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
// Member 'pose'
#include "geometry_msgs/msg/detail/pose__struct.hpp"
// Member 'image'
#include "sensor_msgs/msg/detail/image__struct.hpp"
// Member 'features'
#include "svo_msgs/msg/detail/feature__struct.hpp"

#ifndef _WIN32
# define DEPRECATED__svo_msgs__msg__DenseInputWithFeatures __attribute__((deprecated))
#else
# define DEPRECATED__svo_msgs__msg__DenseInputWithFeatures __declspec(deprecated)
#endif

namespace svo_msgs
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct DenseInputWithFeatures_
{
  using Type = DenseInputWithFeatures_<ContainerAllocator>;

  explicit DenseInputWithFeatures_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : header(_init),
    pose(_init),
    image(_init)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->frame_id = 0ul;
      this->min_depth = 0.0f;
      this->max_depth = 0.0f;
    }
  }

  explicit DenseInputWithFeatures_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : header(_alloc, _init),
    pose(_alloc, _init),
    image(_alloc, _init)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->frame_id = 0ul;
      this->min_depth = 0.0f;
      this->max_depth = 0.0f;
    }
  }

  // field types and members
  using _header_type =
    std_msgs::msg::Header_<ContainerAllocator>;
  _header_type header;
  using _frame_id_type =
    uint32_t;
  _frame_id_type frame_id;
  using _pose_type =
    geometry_msgs::msg::Pose_<ContainerAllocator>;
  _pose_type pose;
  using _image_type =
    sensor_msgs::msg::Image_<ContainerAllocator>;
  _image_type image;
  using _min_depth_type =
    float;
  _min_depth_type min_depth;
  using _max_depth_type =
    float;
  _max_depth_type max_depth;
  using _features_type =
    std::vector<svo_msgs::msg::Feature_<ContainerAllocator>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<svo_msgs::msg::Feature_<ContainerAllocator>>>;
  _features_type features;

  // setters for named parameter idiom
  Type & set__header(
    const std_msgs::msg::Header_<ContainerAllocator> & _arg)
  {
    this->header = _arg;
    return *this;
  }
  Type & set__frame_id(
    const uint32_t & _arg)
  {
    this->frame_id = _arg;
    return *this;
  }
  Type & set__pose(
    const geometry_msgs::msg::Pose_<ContainerAllocator> & _arg)
  {
    this->pose = _arg;
    return *this;
  }
  Type & set__image(
    const sensor_msgs::msg::Image_<ContainerAllocator> & _arg)
  {
    this->image = _arg;
    return *this;
  }
  Type & set__min_depth(
    const float & _arg)
  {
    this->min_depth = _arg;
    return *this;
  }
  Type & set__max_depth(
    const float & _arg)
  {
    this->max_depth = _arg;
    return *this;
  }
  Type & set__features(
    const std::vector<svo_msgs::msg::Feature_<ContainerAllocator>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<svo_msgs::msg::Feature_<ContainerAllocator>>> & _arg)
  {
    this->features = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    svo_msgs::msg::DenseInputWithFeatures_<ContainerAllocator> *;
  using ConstRawPtr =
    const svo_msgs::msg::DenseInputWithFeatures_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<svo_msgs::msg::DenseInputWithFeatures_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<svo_msgs::msg::DenseInputWithFeatures_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      svo_msgs::msg::DenseInputWithFeatures_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<svo_msgs::msg::DenseInputWithFeatures_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      svo_msgs::msg::DenseInputWithFeatures_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<svo_msgs::msg::DenseInputWithFeatures_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<svo_msgs::msg::DenseInputWithFeatures_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<svo_msgs::msg::DenseInputWithFeatures_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__svo_msgs__msg__DenseInputWithFeatures
    std::shared_ptr<svo_msgs::msg::DenseInputWithFeatures_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__svo_msgs__msg__DenseInputWithFeatures
    std::shared_ptr<svo_msgs::msg::DenseInputWithFeatures_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const DenseInputWithFeatures_ & other) const
  {
    if (this->header != other.header) {
      return false;
    }
    if (this->frame_id != other.frame_id) {
      return false;
    }
    if (this->pose != other.pose) {
      return false;
    }
    if (this->image != other.image) {
      return false;
    }
    if (this->min_depth != other.min_depth) {
      return false;
    }
    if (this->max_depth != other.max_depth) {
      return false;
    }
    if (this->features != other.features) {
      return false;
    }
    return true;
  }
  bool operator!=(const DenseInputWithFeatures_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct DenseInputWithFeatures_

// alias to use template instance with default allocator
using DenseInputWithFeatures =
  svo_msgs::msg::DenseInputWithFeatures_<std::allocator<void>>;

// constant definitions

}  // namespace msg

}  // namespace svo_msgs

#endif  // SVO_MSGS__MSG__DETAIL__DENSE_INPUT_WITH_FEATURES__STRUCT_HPP_
