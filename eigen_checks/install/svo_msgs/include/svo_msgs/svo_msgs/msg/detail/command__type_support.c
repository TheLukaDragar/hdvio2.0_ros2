// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from svo_msgs:msg/Command.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "svo_msgs/msg/detail/command__rosidl_typesupport_introspection_c.h"
#include "svo_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "svo_msgs/msg/detail/command__functions.h"
#include "svo_msgs/msg/detail/command__struct.h"


// Include directives for member types
// Member `header`
#include "std_msgs/msg/header.h"
// Member `header`
#include "std_msgs/msg/detail/header__rosidl_typesupport_introspection_c.h"
// Member `bodyrates`
#include "geometry_msgs/msg/vector3.h"
// Member `bodyrates`
#include "geometry_msgs/msg/detail/vector3__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void svo_msgs__msg__Command__rosidl_typesupport_introspection_c__Command_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  svo_msgs__msg__Command__init(message_memory);
}

void svo_msgs__msg__Command__rosidl_typesupport_introspection_c__Command_fini_function(void * message_memory)
{
  svo_msgs__msg__Command__fini(message_memory);
}

size_t svo_msgs__msg__Command__rosidl_typesupport_introspection_c__size_function__Command__thrusts(
  const void * untyped_member)
{
  (void)untyped_member;
  return 4;
}

const void * svo_msgs__msg__Command__rosidl_typesupport_introspection_c__get_const_function__Command__thrusts(
  const void * untyped_member, size_t index)
{
  const double * member =
    (const double *)(untyped_member);
  return &member[index];
}

void * svo_msgs__msg__Command__rosidl_typesupport_introspection_c__get_function__Command__thrusts(
  void * untyped_member, size_t index)
{
  double * member =
    (double *)(untyped_member);
  return &member[index];
}

void svo_msgs__msg__Command__rosidl_typesupport_introspection_c__fetch_function__Command__thrusts(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const double * item =
    ((const double *)
    svo_msgs__msg__Command__rosidl_typesupport_introspection_c__get_const_function__Command__thrusts(untyped_member, index));
  double * value =
    (double *)(untyped_value);
  *value = *item;
}

void svo_msgs__msg__Command__rosidl_typesupport_introspection_c__assign_function__Command__thrusts(
  void * untyped_member, size_t index, const void * untyped_value)
{
  double * item =
    ((double *)
    svo_msgs__msg__Command__rosidl_typesupport_introspection_c__get_function__Command__thrusts(untyped_member, index));
  const double * value =
    (const double *)(untyped_value);
  *item = *value;
}

static rosidl_typesupport_introspection_c__MessageMember svo_msgs__msg__Command__rosidl_typesupport_introspection_c__Command_message_member_array[6] = {
  {
    "header",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(svo_msgs__msg__Command, header),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "t",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_DOUBLE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(svo_msgs__msg__Command, t),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "is_single_rotor_thrust",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_BOOLEAN,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(svo_msgs__msg__Command, is_single_rotor_thrust),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "collective_thrust",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_DOUBLE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(svo_msgs__msg__Command, collective_thrust),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "bodyrates",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(svo_msgs__msg__Command, bodyrates),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "thrusts",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_DOUBLE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    true,  // is array
    4,  // array size
    false,  // is upper bound
    offsetof(svo_msgs__msg__Command, thrusts),  // bytes offset in struct
    NULL,  // default value
    svo_msgs__msg__Command__rosidl_typesupport_introspection_c__size_function__Command__thrusts,  // size() function pointer
    svo_msgs__msg__Command__rosidl_typesupport_introspection_c__get_const_function__Command__thrusts,  // get_const(index) function pointer
    svo_msgs__msg__Command__rosidl_typesupport_introspection_c__get_function__Command__thrusts,  // get(index) function pointer
    svo_msgs__msg__Command__rosidl_typesupport_introspection_c__fetch_function__Command__thrusts,  // fetch(index, &value) function pointer
    svo_msgs__msg__Command__rosidl_typesupport_introspection_c__assign_function__Command__thrusts,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers svo_msgs__msg__Command__rosidl_typesupport_introspection_c__Command_message_members = {
  "svo_msgs__msg",  // message namespace
  "Command",  // message name
  6,  // number of fields
  sizeof(svo_msgs__msg__Command),
  svo_msgs__msg__Command__rosidl_typesupport_introspection_c__Command_message_member_array,  // message members
  svo_msgs__msg__Command__rosidl_typesupport_introspection_c__Command_init_function,  // function to initialize message memory (memory has to be allocated)
  svo_msgs__msg__Command__rosidl_typesupport_introspection_c__Command_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t svo_msgs__msg__Command__rosidl_typesupport_introspection_c__Command_message_type_support_handle = {
  0,
  &svo_msgs__msg__Command__rosidl_typesupport_introspection_c__Command_message_members,
  get_message_typesupport_handle_function,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_svo_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, svo_msgs, msg, Command)() {
  svo_msgs__msg__Command__rosidl_typesupport_introspection_c__Command_message_member_array[0].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, std_msgs, msg, Header)();
  svo_msgs__msg__Command__rosidl_typesupport_introspection_c__Command_message_member_array[4].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, geometry_msgs, msg, Vector3)();
  if (!svo_msgs__msg__Command__rosidl_typesupport_introspection_c__Command_message_type_support_handle.typesupport_identifier) {
    svo_msgs__msg__Command__rosidl_typesupport_introspection_c__Command_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &svo_msgs__msg__Command__rosidl_typesupport_introspection_c__Command_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
