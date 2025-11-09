// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from svo_msgs:msg/DenseInputWithFeatures.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "svo_msgs/msg/detail/dense_input_with_features__rosidl_typesupport_introspection_c.h"
#include "svo_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "svo_msgs/msg/detail/dense_input_with_features__functions.h"
#include "svo_msgs/msg/detail/dense_input_with_features__struct.h"


// Include directives for member types
// Member `header`
#include "std_msgs/msg/header.h"
// Member `header`
#include "std_msgs/msg/detail/header__rosidl_typesupport_introspection_c.h"
// Member `pose`
#include "geometry_msgs/msg/pose.h"
// Member `pose`
#include "geometry_msgs/msg/detail/pose__rosidl_typesupport_introspection_c.h"
// Member `image`
#include "sensor_msgs/msg/image.h"
// Member `image`
#include "sensor_msgs/msg/detail/image__rosidl_typesupport_introspection_c.h"
// Member `features`
#include "svo_msgs/msg/feature.h"
// Member `features`
#include "svo_msgs/msg/detail/feature__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void svo_msgs__msg__DenseInputWithFeatures__rosidl_typesupport_introspection_c__DenseInputWithFeatures_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  svo_msgs__msg__DenseInputWithFeatures__init(message_memory);
}

void svo_msgs__msg__DenseInputWithFeatures__rosidl_typesupport_introspection_c__DenseInputWithFeatures_fini_function(void * message_memory)
{
  svo_msgs__msg__DenseInputWithFeatures__fini(message_memory);
}

size_t svo_msgs__msg__DenseInputWithFeatures__rosidl_typesupport_introspection_c__size_function__DenseInputWithFeatures__features(
  const void * untyped_member)
{
  const svo_msgs__msg__Feature__Sequence * member =
    (const svo_msgs__msg__Feature__Sequence *)(untyped_member);
  return member->size;
}

const void * svo_msgs__msg__DenseInputWithFeatures__rosidl_typesupport_introspection_c__get_const_function__DenseInputWithFeatures__features(
  const void * untyped_member, size_t index)
{
  const svo_msgs__msg__Feature__Sequence * member =
    (const svo_msgs__msg__Feature__Sequence *)(untyped_member);
  return &member->data[index];
}

void * svo_msgs__msg__DenseInputWithFeatures__rosidl_typesupport_introspection_c__get_function__DenseInputWithFeatures__features(
  void * untyped_member, size_t index)
{
  svo_msgs__msg__Feature__Sequence * member =
    (svo_msgs__msg__Feature__Sequence *)(untyped_member);
  return &member->data[index];
}

void svo_msgs__msg__DenseInputWithFeatures__rosidl_typesupport_introspection_c__fetch_function__DenseInputWithFeatures__features(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const svo_msgs__msg__Feature * item =
    ((const svo_msgs__msg__Feature *)
    svo_msgs__msg__DenseInputWithFeatures__rosidl_typesupport_introspection_c__get_const_function__DenseInputWithFeatures__features(untyped_member, index));
  svo_msgs__msg__Feature * value =
    (svo_msgs__msg__Feature *)(untyped_value);
  *value = *item;
}

void svo_msgs__msg__DenseInputWithFeatures__rosidl_typesupport_introspection_c__assign_function__DenseInputWithFeatures__features(
  void * untyped_member, size_t index, const void * untyped_value)
{
  svo_msgs__msg__Feature * item =
    ((svo_msgs__msg__Feature *)
    svo_msgs__msg__DenseInputWithFeatures__rosidl_typesupport_introspection_c__get_function__DenseInputWithFeatures__features(untyped_member, index));
  const svo_msgs__msg__Feature * value =
    (const svo_msgs__msg__Feature *)(untyped_value);
  *item = *value;
}

bool svo_msgs__msg__DenseInputWithFeatures__rosidl_typesupport_introspection_c__resize_function__DenseInputWithFeatures__features(
  void * untyped_member, size_t size)
{
  svo_msgs__msg__Feature__Sequence * member =
    (svo_msgs__msg__Feature__Sequence *)(untyped_member);
  svo_msgs__msg__Feature__Sequence__fini(member);
  return svo_msgs__msg__Feature__Sequence__init(member, size);
}

static rosidl_typesupport_introspection_c__MessageMember svo_msgs__msg__DenseInputWithFeatures__rosidl_typesupport_introspection_c__DenseInputWithFeatures_message_member_array[7] = {
  {
    "header",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(svo_msgs__msg__DenseInputWithFeatures, header),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "frame_id",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT32,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(svo_msgs__msg__DenseInputWithFeatures, frame_id),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "pose",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(svo_msgs__msg__DenseInputWithFeatures, pose),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "image",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(svo_msgs__msg__DenseInputWithFeatures, image),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "min_depth",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_FLOAT,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(svo_msgs__msg__DenseInputWithFeatures, min_depth),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "max_depth",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_FLOAT,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(svo_msgs__msg__DenseInputWithFeatures, max_depth),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "features",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(svo_msgs__msg__DenseInputWithFeatures, features),  // bytes offset in struct
    NULL,  // default value
    svo_msgs__msg__DenseInputWithFeatures__rosidl_typesupport_introspection_c__size_function__DenseInputWithFeatures__features,  // size() function pointer
    svo_msgs__msg__DenseInputWithFeatures__rosidl_typesupport_introspection_c__get_const_function__DenseInputWithFeatures__features,  // get_const(index) function pointer
    svo_msgs__msg__DenseInputWithFeatures__rosidl_typesupport_introspection_c__get_function__DenseInputWithFeatures__features,  // get(index) function pointer
    svo_msgs__msg__DenseInputWithFeatures__rosidl_typesupport_introspection_c__fetch_function__DenseInputWithFeatures__features,  // fetch(index, &value) function pointer
    svo_msgs__msg__DenseInputWithFeatures__rosidl_typesupport_introspection_c__assign_function__DenseInputWithFeatures__features,  // assign(index, value) function pointer
    svo_msgs__msg__DenseInputWithFeatures__rosidl_typesupport_introspection_c__resize_function__DenseInputWithFeatures__features  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers svo_msgs__msg__DenseInputWithFeatures__rosidl_typesupport_introspection_c__DenseInputWithFeatures_message_members = {
  "svo_msgs__msg",  // message namespace
  "DenseInputWithFeatures",  // message name
  7,  // number of fields
  sizeof(svo_msgs__msg__DenseInputWithFeatures),
  svo_msgs__msg__DenseInputWithFeatures__rosidl_typesupport_introspection_c__DenseInputWithFeatures_message_member_array,  // message members
  svo_msgs__msg__DenseInputWithFeatures__rosidl_typesupport_introspection_c__DenseInputWithFeatures_init_function,  // function to initialize message memory (memory has to be allocated)
  svo_msgs__msg__DenseInputWithFeatures__rosidl_typesupport_introspection_c__DenseInputWithFeatures_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t svo_msgs__msg__DenseInputWithFeatures__rosidl_typesupport_introspection_c__DenseInputWithFeatures_message_type_support_handle = {
  0,
  &svo_msgs__msg__DenseInputWithFeatures__rosidl_typesupport_introspection_c__DenseInputWithFeatures_message_members,
  get_message_typesupport_handle_function,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_svo_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, svo_msgs, msg, DenseInputWithFeatures)() {
  svo_msgs__msg__DenseInputWithFeatures__rosidl_typesupport_introspection_c__DenseInputWithFeatures_message_member_array[0].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, std_msgs, msg, Header)();
  svo_msgs__msg__DenseInputWithFeatures__rosidl_typesupport_introspection_c__DenseInputWithFeatures_message_member_array[2].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, geometry_msgs, msg, Pose)();
  svo_msgs__msg__DenseInputWithFeatures__rosidl_typesupport_introspection_c__DenseInputWithFeatures_message_member_array[3].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, sensor_msgs, msg, Image)();
  svo_msgs__msg__DenseInputWithFeatures__rosidl_typesupport_introspection_c__DenseInputWithFeatures_message_member_array[6].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, svo_msgs, msg, Feature)();
  if (!svo_msgs__msg__DenseInputWithFeatures__rosidl_typesupport_introspection_c__DenseInputWithFeatures_message_type_support_handle.typesupport_identifier) {
    svo_msgs__msg__DenseInputWithFeatures__rosidl_typesupport_introspection_c__DenseInputWithFeatures_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &svo_msgs__msg__DenseInputWithFeatures__rosidl_typesupport_introspection_c__DenseInputWithFeatures_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
