// generated from rosidl_typesupport_introspection_cpp/resource/idl__type_support.cpp.em
// with input from svo_msgs:msg/NbvTrajectory.idl
// generated code does not contain a copyright notice

#include "array"
#include "cstddef"
#include "string"
#include "vector"
#include "rosidl_runtime_c/message_type_support_struct.h"
#include "rosidl_typesupport_cpp/message_type_support.hpp"
#include "rosidl_typesupport_interface/macros.h"
#include "svo_msgs/msg/detail/nbv_trajectory__struct.hpp"
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

void NbvTrajectory_init_function(
  void * message_memory, rosidl_runtime_cpp::MessageInitialization _init)
{
  new (message_memory) svo_msgs::msg::NbvTrajectory(_init);
}

void NbvTrajectory_fini_function(void * message_memory)
{
  auto typed_message = static_cast<svo_msgs::msg::NbvTrajectory *>(message_memory);
  typed_message->~NbvTrajectory();
}

size_t size_function__NbvTrajectory__trajectory(const void * untyped_member)
{
  const auto * member = reinterpret_cast<const std::vector<geometry_msgs::msg::Pose> *>(untyped_member);
  return member->size();
}

const void * get_const_function__NbvTrajectory__trajectory(const void * untyped_member, size_t index)
{
  const auto & member =
    *reinterpret_cast<const std::vector<geometry_msgs::msg::Pose> *>(untyped_member);
  return &member[index];
}

void * get_function__NbvTrajectory__trajectory(void * untyped_member, size_t index)
{
  auto & member =
    *reinterpret_cast<std::vector<geometry_msgs::msg::Pose> *>(untyped_member);
  return &member[index];
}

void fetch_function__NbvTrajectory__trajectory(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const auto & item = *reinterpret_cast<const geometry_msgs::msg::Pose *>(
    get_const_function__NbvTrajectory__trajectory(untyped_member, index));
  auto & value = *reinterpret_cast<geometry_msgs::msg::Pose *>(untyped_value);
  value = item;
}

void assign_function__NbvTrajectory__trajectory(
  void * untyped_member, size_t index, const void * untyped_value)
{
  auto & item = *reinterpret_cast<geometry_msgs::msg::Pose *>(
    get_function__NbvTrajectory__trajectory(untyped_member, index));
  const auto & value = *reinterpret_cast<const geometry_msgs::msg::Pose *>(untyped_value);
  item = value;
}

void resize_function__NbvTrajectory__trajectory(void * untyped_member, size_t size)
{
  auto * member =
    reinterpret_cast<std::vector<geometry_msgs::msg::Pose> *>(untyped_member);
  member->resize(size);
}

static const ::rosidl_typesupport_introspection_cpp::MessageMember NbvTrajectory_message_member_array[2] = {
  {
    "header",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    ::rosidl_typesupport_introspection_cpp::get_message_type_support_handle<std_msgs::msg::Header>(),  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(svo_msgs::msg::NbvTrajectory, header),  // bytes offset in struct
    nullptr,  // default value
    nullptr,  // size() function pointer
    nullptr,  // get_const(index) function pointer
    nullptr,  // get(index) function pointer
    nullptr,  // fetch(index, &value) function pointer
    nullptr,  // assign(index, value) function pointer
    nullptr  // resize(index) function pointer
  },
  {
    "trajectory",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    ::rosidl_typesupport_introspection_cpp::get_message_type_support_handle<geometry_msgs::msg::Pose>(),  // members of sub message
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(svo_msgs::msg::NbvTrajectory, trajectory),  // bytes offset in struct
    nullptr,  // default value
    size_function__NbvTrajectory__trajectory,  // size() function pointer
    get_const_function__NbvTrajectory__trajectory,  // get_const(index) function pointer
    get_function__NbvTrajectory__trajectory,  // get(index) function pointer
    fetch_function__NbvTrajectory__trajectory,  // fetch(index, &value) function pointer
    assign_function__NbvTrajectory__trajectory,  // assign(index, value) function pointer
    resize_function__NbvTrajectory__trajectory  // resize(index) function pointer
  }
};

static const ::rosidl_typesupport_introspection_cpp::MessageMembers NbvTrajectory_message_members = {
  "svo_msgs::msg",  // message namespace
  "NbvTrajectory",  // message name
  2,  // number of fields
  sizeof(svo_msgs::msg::NbvTrajectory),
  NbvTrajectory_message_member_array,  // message members
  NbvTrajectory_init_function,  // function to initialize message memory (memory has to be allocated)
  NbvTrajectory_fini_function  // function to terminate message instance (will not free memory)
};

static const rosidl_message_type_support_t NbvTrajectory_message_type_support_handle = {
  ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  &NbvTrajectory_message_members,
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
get_message_type_support_handle<svo_msgs::msg::NbvTrajectory>()
{
  return &::svo_msgs::msg::rosidl_typesupport_introspection_cpp::NbvTrajectory_message_type_support_handle;
}

}  // namespace rosidl_typesupport_introspection_cpp

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_INTROSPECTION_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, svo_msgs, msg, NbvTrajectory)() {
  return &::svo_msgs::msg::rosidl_typesupport_introspection_cpp::NbvTrajectory_message_type_support_handle;
}

#ifdef __cplusplus
}
#endif
