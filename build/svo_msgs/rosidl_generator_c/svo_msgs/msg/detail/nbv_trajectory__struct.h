// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from svo_msgs:msg/NbvTrajectory.idl
// generated code does not contain a copyright notice

#ifndef SVO_MSGS__MSG__DETAIL__NBV_TRAJECTORY__STRUCT_H_
#define SVO_MSGS__MSG__DETAIL__NBV_TRAJECTORY__STRUCT_H_

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
// Member 'trajectory'
#include "geometry_msgs/msg/detail/pose__struct.h"

/// Struct defined in msg/NbvTrajectory in the package svo_msgs.
typedef struct svo_msgs__msg__NbvTrajectory
{
  std_msgs__msg__Header header;
  geometry_msgs__msg__Pose__Sequence trajectory;
} svo_msgs__msg__NbvTrajectory;

// Struct for a sequence of svo_msgs__msg__NbvTrajectory.
typedef struct svo_msgs__msg__NbvTrajectory__Sequence
{
  svo_msgs__msg__NbvTrajectory * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} svo_msgs__msg__NbvTrajectory__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // SVO_MSGS__MSG__DETAIL__NBV_TRAJECTORY__STRUCT_H_
