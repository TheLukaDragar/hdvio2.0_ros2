// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from svo_msgs:msg/Feature.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "svo_msgs/msg/detail/feature__rosidl_typesupport_introspection_c.h"
#include "svo_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "svo_msgs/msg/detail/feature__functions.h"
#include "svo_msgs/msg/detail/feature__struct.h"


#ifdef __cplusplus
extern "C"
{
#endif

void svo_msgs__msg__Feature__rosidl_typesupport_introspection_c__Feature_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  svo_msgs__msg__Feature__init(message_memory);
}

void svo_msgs__msg__Feature__rosidl_typesupport_introspection_c__Feature_fini_function(void * message_memory)
{
  svo_msgs__msg__Feature__fini(message_memory);
}

static rosidl_typesupport_introspection_c__MessageMember svo_msgs__msg__Feature__rosidl_typesupport_introspection_c__Feature_message_member_array[3] = {
  {
    "x",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_FLOAT,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(svo_msgs__msg__Feature, x),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "y",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_FLOAT,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(svo_msgs__msg__Feature, y),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "z",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_FLOAT,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(svo_msgs__msg__Feature, z),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers svo_msgs__msg__Feature__rosidl_typesupport_introspection_c__Feature_message_members = {
  "svo_msgs__msg",  // message namespace
  "Feature",  // message name
  3,  // number of fields
  sizeof(svo_msgs__msg__Feature),
  svo_msgs__msg__Feature__rosidl_typesupport_introspection_c__Feature_message_member_array,  // message members
  svo_msgs__msg__Feature__rosidl_typesupport_introspection_c__Feature_init_function,  // function to initialize message memory (memory has to be allocated)
  svo_msgs__msg__Feature__rosidl_typesupport_introspection_c__Feature_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t svo_msgs__msg__Feature__rosidl_typesupport_introspection_c__Feature_message_type_support_handle = {
  0,
  &svo_msgs__msg__Feature__rosidl_typesupport_introspection_c__Feature_message_members,
  get_message_typesupport_handle_function,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_svo_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, svo_msgs, msg, Feature)() {
  if (!svo_msgs__msg__Feature__rosidl_typesupport_introspection_c__Feature_message_type_support_handle.typesupport_identifier) {
    svo_msgs__msg__Feature__rosidl_typesupport_introspection_c__Feature_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &svo_msgs__msg__Feature__rosidl_typesupport_introspection_c__Feature_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
