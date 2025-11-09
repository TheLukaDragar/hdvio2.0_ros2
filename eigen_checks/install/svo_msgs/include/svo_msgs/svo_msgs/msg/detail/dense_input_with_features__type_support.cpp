// generated from rosidl_typesupport_introspection_cpp/resource/idl__type_support.cpp.em
// with input from svo_msgs:msg/DenseInputWithFeatures.idl
// generated code does not contain a copyright notice

#include "array"
#include "cstddef"
#include "string"
#include "vector"
#include "rosidl_runtime_c/message_type_support_struct.h"
#include "rosidl_typesupport_cpp/message_type_support.hpp"
#include "rosidl_typesupport_interface/macros.h"
#include "svo_msgs/msg/detail/dense_input_with_features__struct.hpp"
#include "rosidl_typesupport_introspection_cpp/field_types.hpp"
#include "rosidl_typesupport_introspection_cpp/identifier.hpp"
#include "rosidl_typesupport_introspection_cpp/message_introspection.hpp"
#include "rosidl_typesupport_introspection_cpp/message_type_support_decl.hpp"
#include "rosidl_typesupport_introspection_cpp/visibility_control.h"

namespace svo_msgs
{

namespace msg
{

namespace rosidl_typesupport_introspection_cpp
{

void DenseInputWithFeatures_init_function(
  void * message_memory, rosidl_runtime_cpp::MessageInitialization _init)
{
  new (message_memory) svo_msgs::msg::DenseInputWithFeatures(_init);
}

void DenseInputWithFeatures_fini_function(void * message_memory)
{
  auto typed_message = static_cast<svo_msgs::msg::DenseInputWithFeatures *>(message_memory);
  typed_message->~DenseInputWithFeatures();
}

size_t size_function__DenseInputWithFeatures__features(const void * untyped_member)
{
  const auto * member = reinterpret_cast<const std::vector<svo_msgs::msg::Feature> *>(untyped_member);
  return member->size();
}

const void * get_const_function__DenseInputWithFeatures__features(const void * untyped_member, size_t index)
{
  const auto & member =
    *reinterpret_cast<const std::vector<svo_msgs::msg::Feature> *>(untyped_member);
  return &member[index];
}

void * get_function__DenseInputWithFeatures__features(void * untyped_member, size_t index)
{
  auto & member =
    *reinterpret_cast<std::vector<svo_msgs::msg::Feature> *>(untyped_member);
  return &member[index];
}

void fetch_function__DenseInputWithFeatures__features(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const auto & item = *reinterpret_cast<const svo_msgs::msg::Feature *>(
    get_const_function__DenseInputWithFeatures__features(untyped_member, index));
  auto & value = *reinterpret_cast<svo_msgs::msg::Feature *>(untyped_value);
  value = item;
}

void assign_function__DenseInputWithFeatures__features(
  void * untyped_member, size_t index, const void * untyped_value)
{
  auto & item = *reinterpret_cast<svo_msgs::msg::Feature *>(
    get_function__DenseInputWithFeatures__features(untyped_member, index));
  const auto & value = *reinterpret_cast<const svo_msgs::msg::Feature *>(untyped_value);
  item = value;
}

void resize_function__DenseInputWithFeatures__features(void * untyped_member, size_t size)
{
  auto * member =
    reinterpret_cast<std::vector<svo_msgs::msg::Feature> *>(untyped_member);
  member->resize(size);
}

static const ::rosidl_typesupport_introspection_cpp::MessageMember DenseInputWithFeatures_message_member_array[7] = {
  {
    "header",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    ::rosidl_typesupport_introspection_cpp::get_message_type_support_handle<std_msgs::msg::Header>(),  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(svo_msgs::msg::DenseInputWithFeatures, header),  // bytes offset in struct
    nullptr,  // default value
    nullptr,  // size() function pointer
    nullptr,  // get_const(index) function pointer
    nullptr,  // get(index) function pointer
    nullptr,  // fetch(index, &value) function pointer
    nullptr,  // assign(index, value) function pointer
    nullptr  // resize(index) function pointer
  },
  {
    "frame_id",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT32,  // type
    0,  // upper bound of string
    nullptr,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(svo_msgs::msg::DenseInputWithFeatures, frame_id),  // bytes offset in struct
    nullptr,  // default value
    nullptr,  // size() function pointer
    nullptr,  // get_const(index) function pointer
    nullptr,  // get(index) function pointer
    nullptr,  // fetch(index, &value) function pointer
    nullptr,  // assign(index, value) function pointer
    nullptr  // resize(index) function pointer
  },
  {
    "pose",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    ::rosidl_typesupport_introspection_cpp::get_message_type_support_handle<geometry_msgs::msg::Pose>(),  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(svo_msgs::msg::DenseInputWithFeatures, pose),  // bytes offset in struct
    nullptr,  // default value
    nullptr,  // size() function pointer
    nullptr,  // get_const(index) function pointer
    nullptr,  // get(index) function pointer
    nullptr,  // fetch(index, &value) function pointer
    nullptr,  // assign(index, value) function pointer
    nullptr  // resize(index) function pointer
  },
  {
    "image",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    ::rosidl_typesupport_introspection_cpp::get_message_type_support_handle<sensor_msgs::msg::Image>(),  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(svo_msgs::msg::DenseInputWithFeatures, image),  // bytes offset in struct
    nullptr,  // default value
    nullptr,  // size() function pointer
    nullptr,  // get_const(index) function pointer
    nullptr,  // get(index) function pointer
    nullptr,  // fetch(index, &value) function pointer
    nullptr,  // assign(index, value) function pointer
    nullptr  // resize(index) function pointer
  },
  {
    "min_depth",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_FLOAT,  // type
    0,  // upper bound of string
    nullptr,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(svo_msgs::msg::DenseInputWithFeatures, min_depth),  // bytes offset in struct
    nullptr,  // default value
    nullptr,  // size() function pointer
    nullptr,  // get_const(index) function pointer
    nullptr,  // get(index) function pointer
    nullptr,  // fetch(index, &value) function pointer
    nullptr,  // assign(index, value) function pointer
    nullptr  // resize(index) function pointer
  },
  {
    "max_depth",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_FLOAT,  // type
    0,  // upper bound of string
    nullptr,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(svo_msgs::msg::DenseInputWithFeatures, max_depth),  // bytes offset in struct
    nullptr,  // default value
    nullptr,  // size() function pointer
    nullptr,  // get_const(index) function pointer
    nullptr,  // get(index) function pointer
    nullptr,  // fetch(index, &value) function pointer
    nullptr,  // assign(index, value) function pointer
    nullptr  // resize(index) function pointer
  },
  {
    "features",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    ::rosidl_typesupport_introspection_cpp::get_message_type_support_handle<svo_msgs::msg::Feature>(),  // members of sub message
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(svo_msgs::msg::DenseInputWithFeatures, features),  // bytes offset in struct
    nullptr,  // default value
    size_function__DenseInputWithFeatures__features,  // size() function pointer
    get_const_function__DenseInputWithFeatures__features,  // get_const(index) function pointer
    get_function__DenseInputWithFeatures__features,  // get(index) function pointer
    fetch_function__DenseInputWithFeatures__features,  // fetch(index, &value) function pointer
    assign_function__DenseInputWithFeatures__features,  // assign(index, value) function pointer
    resize_function__DenseInputWithFeatures__features  // resize(index) function pointer
  }
};

static const ::rosidl_typesupport_introspection_cpp::MessageMembers DenseInputWithFeatures_message_members = {
  "svo_msgs::msg",  // message namespace
  "DenseInputWithFeatures",  // message name
  7,  // number of fields
  sizeof(svo_msgs::msg::DenseInputWithFeatures),
  DenseInputWithFeatures_message_member_array,  // message members
  DenseInputWithFeatures_init_function,  // function to initialize message memory (memory has to be allocated)
  DenseInputWithFeatures_fini_function  // function to terminate message instance (will not free memory)
};

static const rosidl_message_type_support_t DenseInputWithFeatures_message_type_support_handle = {
  ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  &DenseInputWithFeatures_message_members,
  get_message_typesupport_handle_function,
};

}  // namespace rosidl_typesupport_introspection_cpp

}  // namespace msg

}  // namespace svo_msgs


namespace rosidl_typesupport_introspection_cpp
{

template<>
ROSIDL_TYPESUPPORT_INTROSPECTION_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<svo_msgs::msg::DenseInputWithFeatures>()
{
  return &::svo_msgs::msg::rosidl_typesupport_introspection_cpp::DenseInputWithFeatures_message_type_support_handle;
}

}  // namespace rosidl_typesupport_introspection_cpp

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_INTROSPECTION_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, svo_msgs, msg, DenseInputWithFeatures)() {
  return &::svo_msgs::msg::rosidl_typesupport_introspection_cpp::DenseInputWithFeatures_message_type_support_handle;
}

#ifdef __cplusplus
}
#endif
