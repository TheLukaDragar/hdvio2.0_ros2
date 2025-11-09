// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from svo_msgs:msg/DenseInputWithFeatures.idl
// generated code does not contain a copyright notice

#ifndef SVO_MSGS__MSG__DETAIL__DENSE_INPUT_WITH_FEATURES__STRUCT_H_
#define SVO_MSGS__MSG__DETAIL__DENSE_INPUT_WITH_FEATURES__STRUCT_H_

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
// Member 'pose'
#include "geometry_msgs/msg/detail/pose__struct.h"
// Member 'image'
#include "sensor_msgs/msg/detail/image__struct.h"
// Member 'features'
#include "svo_msgs/msg/detail/feature__struct.h"

/// Struct defined in msg/DenseInputWithFeatures in the package svo_msgs.
typedef struct svo_msgs__msg__DenseInputWithFeatures
{
  std_msgs__msg__Header header;
  uint32_t frame_id;
  geometry_msgs__msg__Pose pose;
  sensor_msgs__msg__Image image;
  float min_depth;
  float max_depth;
  svo_msgs__msg__Feature__Sequence features;
} svo_msgs__msg__DenseInputWithFeatures;

// Struct for a sequence of svo_msgs__msg__DenseInputWithFeatures.
typedef struct svo_msgs__msg__DenseInputWithFeatures__Sequence
{
  svo_msgs__msg__DenseInputWithFeatures * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} svo_msgs__msg__DenseInputWithFeatures__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // SVO_MSGS__MSG__DETAIL__DENSE_INPUT_WITH_FEATURES__STRUCT_H_
