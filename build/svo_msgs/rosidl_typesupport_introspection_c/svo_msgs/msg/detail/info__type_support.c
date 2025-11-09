// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from svo_msgs:msg/Info.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "svo_msgs/msg/detail/info__rosidl_typesupport_introspection_c.h"
#include "svo_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "svo_msgs/msg/detail/info__functions.h"
#include "svo_msgs/msg/detail/info__struct.h"


// Include directives for member types
// Member `header`
#include "std_msgs/msg/header.h"
// Member `header`
#include "std_msgs/msg/detail/header__rosidl_typesupport_introspection_c.h"
// Member `keyframes`
#include "rosidl_runtime_c/primitives_sequence_functions.h"

#ifdef __cplusplus
extern "C"
{
#endif

void svo_msgs__msg__Info__rosidl_typesupport_introspection_c__Info_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  svo_msgs__msg__Info__init(message_memory);
}

void svo_msgs__msg__Info__rosidl_typesupport_introspection_c__Info_fini_function(void * message_memory)
{
  svo_msgs__msg__Info__fini(message_memory);
}

size_t svo_msgs__msg__Info__rosidl_typesupport_introspection_c__size_function__Info__keyframes(
  const void * untyped_member)
{
  const rosidl_runtime_c__int32__Sequence * member =
    (const rosidl_runtime_c__int32__Sequence *)(untyped_member);
  return member->size;
}

const void * svo_msgs__msg__Info__rosidl_typesupport_introspection_c__get_const_function__Info__keyframes(
  const void * untyped_member, size_t index)
{
  const rosidl_runtime_c__int32__Sequence * member =
    (const rosidl_runtime_c__int32__Sequence *)(untyped_member);
  return &member->data[index];
}

void * svo_msgs__msg__Info__rosidl_typesupport_introspection_c__get_function__Info__keyframes(
  void * untyped_member, size_t index)
{
  rosidl_runtime_c__int32__Sequence * member =
    (rosidl_runtime_c__int32__Sequence *)(untyped_member);
  return &member->data[index];
}

void svo_msgs__msg__Info__rosidl_typesupport_introspection_c__fetch_function__Info__keyframes(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const int32_t * item =
    ((const int32_t *)
    svo_msgs__msg__Info__rosidl_typesupport_introspection_c__get_const_function__Info__keyframes(untyped_member, index));
  int32_t * value =
    (int32_t *)(untyped_value);
  *value = *item;
}

void svo_msgs__msg__Info__rosidl_typesupport_introspection_c__assign_function__Info__keyframes(
  void * untyped_member, size_t index, const void * untyped_value)
{
  int32_t * item =
    ((int32_t *)
    svo_msgs__msg__Info__rosidl_typesupport_introspection_c__get_function__Info__keyframes(untyped_member, index));
  const int32_t * value =
    (const int32_t *)(untyped_value);
  *item = *value;
}

bool svo_msgs__msg__Info__rosidl_typesupport_introspection_c__resize_function__Info__keyframes(
  void * untyped_member, size_t size)
{
  rosidl_runtime_c__int32__Sequence * member =
    (rosidl_runtime_c__int32__Sequence *)(untyped_member);
  rosidl_runtime_c__int32__Sequence__fini(member);
  return rosidl_runtime_c__int32__Sequence__init(member, size);
}

static rosidl_typesupport_introspection_c__MessageMember svo_msgs__msg__Info__rosidl_typesupport_introspection_c__Info_message_member_array[6] = {
  {
    "header",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(svo_msgs__msg__Info, header),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "processing_time",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_FLOAT,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(svo_msgs__msg__Info, processing_time),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "keyframes",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_INT32,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(svo_msgs__msg__Info, keyframes),  // bytes offset in struct
    NULL,  // default value
    svo_msgs__msg__Info__rosidl_typesupport_introspection_c__size_function__Info__keyframes,  // size() function pointer
    svo_msgs__msg__Info__rosidl_typesupport_introspection_c__get_const_function__Info__keyframes,  // get_const(index) function pointer
    svo_msgs__msg__Info__rosidl_typesupport_introspection_c__get_function__Info__keyframes,  // get(index) function pointer
    svo_msgs__msg__Info__rosidl_typesupport_introspection_c__fetch_function__Info__keyframes,  // fetch(index, &value) function pointer
    svo_msgs__msg__Info__rosidl_typesupport_introspection_c__assign_function__Info__keyframes,  // assign(index, value) function pointer
    svo_msgs__msg__Info__rosidl_typesupport_introspection_c__resize_function__Info__keyframes  // resize(index) function pointer
  },
  {
    "num_matches",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_INT32,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(svo_msgs__msg__Info, num_matches),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "tracking_quality",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_INT32,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(svo_msgs__msg__Info, tracking_quality),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "stage",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_INT32,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(svo_msgs__msg__Info, stage),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers svo_msgs__msg__Info__rosidl_typesupport_introspection_c__Info_message_members = {
  "svo_msgs__msg",  // message namespace
  "Info",  // message name
  6,  // number of fields
  sizeof(svo_msgs__msg__Info),
  svo_msgs__msg__Info__rosidl_typesupport_introspection_c__Info_message_member_array,  // message members
  svo_msgs__msg__Info__rosidl_typesupport_introspection_c__Info_init_function,  // function to initialize message memory (memory has to be allocated)
  svo_msgs__msg__Info__rosidl_typesupport_introspection_c__Info_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t svo_msgs__msg__Info__rosidl_typesupport_introspection_c__Info_message_type_support_handle = {
  0,
  &svo_msgs__msg__Info__rosidl_typesupport_introspection_c__Info_message_members,
  get_message_typesupport_handle_function,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_svo_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, svo_msgs, msg, Info)() {
  svo_msgs__msg__Info__rosidl_typesupport_introspection_c__Info_message_member_array[0].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, std_msgs, msg, Header)();
  if (!svo_msgs__msg__Info__rosidl_typesupport_introspection_c__Info_message_type_support_handle.typesupport_identifier) {
    svo_msgs__msg__Info__rosidl_typesupport_introspection_c__Info_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &svo_msgs__msg__Info__rosidl_typesupport_introspection_c__Info_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
