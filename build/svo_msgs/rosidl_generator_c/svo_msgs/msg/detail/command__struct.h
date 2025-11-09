// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from svo_msgs:msg/Command.idl
// generated code does not contain a copyright notice

#ifndef SVO_MSGS__MSG__DETAIL__COMMAND__STRUCT_H_
#define SVO_MSGS__MSG__DETAIL__COMMAND__STRUCT_H_

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
// Member 'bodyrates'
#include "geometry_msgs/msg/detail/vector3__struct.h"

/// Struct defined in msg/Command in the package svo_msgs.
typedef struct svo_msgs__msg__Command
{
  std_msgs__msg__Header header;
  /// Time
  double t;
  /// If input is in terms of collective thrust & bodyrates or single rotor thrusts
  bool is_single_rotor_thrust;
  /// Collective mass-normalized thrust
  double collective_thrust;
  /// Angular rates in body frame
  geometry_msgs__msg__Vector3 bodyrates;
  /// Single rotor thrusts
  double thrusts[4];
} svo_msgs__msg__Command;

// Struct for a sequence of svo_msgs__msg__Command.
typedef struct svo_msgs__msg__Command__Sequence
{
  svo_msgs__msg__Command * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} svo_msgs__msg__Command__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // SVO_MSGS__MSG__DETAIL__COMMAND__STRUCT_H_
