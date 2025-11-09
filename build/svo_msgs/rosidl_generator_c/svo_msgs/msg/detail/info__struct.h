// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from svo_msgs:msg/Info.idl
// generated code does not contain a copyright notice

#ifndef SVO_MSGS__MSG__DETAIL__INFO__STRUCT_H_
#define SVO_MSGS__MSG__DETAIL__INFO__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'header'
#include "std_msgs/msg/detail/header__struct.h"
// Member 'keyframes'
#include "rosidl_runtime_c/primitives_sequence.h"

/// Struct defined in msg/Info in the package svo_msgs.
typedef struct svo_msgs__msg__Info
{
  std_msgs__msg__Header header;
  float processing_time;
  rosidl_runtime_c__int32__Sequence keyframes;
  int32_t num_matches;
  int32_t tracking_quality;
  int32_t stage;
} svo_msgs__msg__Info;

// Struct for a sequence of svo_msgs__msg__Info.
typedef struct svo_msgs__msg__Info__Sequence
{
  svo_msgs__msg__Info * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} svo_msgs__msg__Info__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // SVO_MSGS__MSG__DETAIL__INFO__STRUCT_H_
