// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from svo_msgs:msg/Feature.idl
// generated code does not contain a copyright notice

#ifndef SVO_MSGS__MSG__DETAIL__FEATURE__STRUCT_H_
#define SVO_MSGS__MSG__DETAIL__FEATURE__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

/// Struct defined in msg/Feature in the package svo_msgs.
typedef struct svo_msgs__msg__Feature
{
  /// x component of 3d point in camera frame
  float x;
  /// y component of 3d point in camera frame
  float y;
  /// z component of 3d point in camera frame
  float z;
} svo_msgs__msg__Feature;

// Struct for a sequence of svo_msgs__msg__Feature.
typedef struct svo_msgs__msg__Feature__Sequence
{
  svo_msgs__msg__Feature * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} svo_msgs__msg__Feature__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // SVO_MSGS__MSG__DETAIL__FEATURE__STRUCT_H_
