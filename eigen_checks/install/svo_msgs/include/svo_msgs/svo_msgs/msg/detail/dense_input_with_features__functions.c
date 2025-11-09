// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from svo_msgs:msg/DenseInputWithFeatures.idl
// generated code does not contain a copyright notice
#include "svo_msgs/msg/detail/dense_input_with_features__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `header`
#include "std_msgs/msg/detail/header__functions.h"
// Member `pose`
#include "geometry_msgs/msg/detail/pose__functions.h"
// Member `image`
#include "sensor_msgs/msg/detail/image__functions.h"
// Member `features`
#include "svo_msgs/msg/detail/feature__functions.h"

bool
svo_msgs__msg__DenseInputWithFeatures__init(svo_msgs__msg__DenseInputWithFeatures * msg)
{
  if (!msg) {
    return false;
  }
  // header
  if (!std_msgs__msg__Header__init(&msg->header)) {
    svo_msgs__msg__DenseInputWithFeatures__fini(msg);
    return false;
  }
  // frame_id
  // pose
  if (!geometry_msgs__msg__Pose__init(&msg->pose)) {
    svo_msgs__msg__DenseInputWithFeatures__fini(msg);
    return false;
  }
  // image
  if (!sensor_msgs__msg__Image__init(&msg->image)) {
    svo_msgs__msg__DenseInputWithFeatures__fini(msg);
    return false;
  }
  // min_depth
  // max_depth
  // features
  if (!svo_msgs__msg__Feature__Sequence__init(&msg->features, 0)) {
    svo_msgs__msg__DenseInputWithFeatures__fini(msg);
    return false;
  }
  return true;
}

void
svo_msgs__msg__DenseInputWithFeatures__fini(svo_msgs__msg__DenseInputWithFeatures * msg)
{
  if (!msg) {
    return;
  }
  // header
  std_msgs__msg__Header__fini(&msg->header);
  // frame_id
  // pose
  geometry_msgs__msg__Pose__fini(&msg->pose);
  // image
  sensor_msgs__msg__Image__fini(&msg->image);
  // min_depth
  // max_depth
  // features
  svo_msgs__msg__Feature__Sequence__fini(&msg->features);
}

bool
svo_msgs__msg__DenseInputWithFeatures__are_equal(const svo_msgs__msg__DenseInputWithFeatures * lhs, const svo_msgs__msg__DenseInputWithFeatures * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // header
  if (!std_msgs__msg__Header__are_equal(
      &(lhs->header), &(rhs->header)))
  {
    return false;
  }
  // frame_id
  if (lhs->frame_id != rhs->frame_id) {
    return false;
  }
  // pose
  if (!geometry_msgs__msg__Pose__are_equal(
      &(lhs->pose), &(rhs->pose)))
  {
    return false;
  }
  // image
  if (!sensor_msgs__msg__Image__are_equal(
      &(lhs->image), &(rhs->image)))
  {
    return false;
  }
  // min_depth
  if (lhs->min_depth != rhs->min_depth) {
    return false;
  }
  // max_depth
  if (lhs->max_depth != rhs->max_depth) {
    return false;
  }
  // features
  if (!svo_msgs__msg__Feature__Sequence__are_equal(
      &(lhs->features), &(rhs->features)))
  {
    return false;
  }
  return true;
}

bool
svo_msgs__msg__DenseInputWithFeatures__copy(
  const svo_msgs__msg__DenseInputWithFeatures * input,
  svo_msgs__msg__DenseInputWithFeatures * output)
{
  if (!input || !output) {
    return false;
  }
  // header
  if (!std_msgs__msg__Header__copy(
      &(input->header), &(output->header)))
  {
    return false;
  }
  // frame_id
  output->frame_id = input->frame_id;
  // pose
  if (!geometry_msgs__msg__Pose__copy(
      &(input->pose), &(output->pose)))
  {
    return false;
  }
  // image
  if (!sensor_msgs__msg__Image__copy(
      &(input->image), &(output->image)))
  {
    return false;
  }
  // min_depth
  output->min_depth = input->min_depth;
  // max_depth
  output->max_depth = input->max_depth;
  // features
  if (!svo_msgs__msg__Feature__Sequence__copy(
      &(input->features), &(output->features)))
  {
    return false;
  }
  return true;
}

svo_msgs__msg__DenseInputWithFeatures *
svo_msgs__msg__DenseInputWithFeatures__create()
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  svo_msgs__msg__DenseInputWithFeatures * msg = (svo_msgs__msg__DenseInputWithFeatures *)allocator.allocate(sizeof(svo_msgs__msg__DenseInputWithFeatures), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(svo_msgs__msg__DenseInputWithFeatures));
  bool success = svo_msgs__msg__DenseInputWithFeatures__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
svo_msgs__msg__DenseInputWithFeatures__destroy(svo_msgs__msg__DenseInputWithFeatures * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    svo_msgs__msg__DenseInputWithFeatures__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
svo_msgs__msg__DenseInputWithFeatures__Sequence__init(svo_msgs__msg__DenseInputWithFeatures__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  svo_msgs__msg__DenseInputWithFeatures * data = NULL;

  if (size) {
    data = (svo_msgs__msg__DenseInputWithFeatures *)allocator.zero_allocate(size, sizeof(svo_msgs__msg__DenseInputWithFeatures), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = svo_msgs__msg__DenseInputWithFeatures__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        svo_msgs__msg__DenseInputWithFeatures__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
svo_msgs__msg__DenseInputWithFeatures__Sequence__fini(svo_msgs__msg__DenseInputWithFeatures__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      svo_msgs__msg__DenseInputWithFeatures__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

svo_msgs__msg__DenseInputWithFeatures__Sequence *
svo_msgs__msg__DenseInputWithFeatures__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  svo_msgs__msg__DenseInputWithFeatures__Sequence * array = (svo_msgs__msg__DenseInputWithFeatures__Sequence *)allocator.allocate(sizeof(svo_msgs__msg__DenseInputWithFeatures__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = svo_msgs__msg__DenseInputWithFeatures__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
svo_msgs__msg__DenseInputWithFeatures__Sequence__destroy(svo_msgs__msg__DenseInputWithFeatures__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    svo_msgs__msg__DenseInputWithFeatures__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
svo_msgs__msg__DenseInputWithFeatures__Sequence__are_equal(const svo_msgs__msg__DenseInputWithFeatures__Sequence * lhs, const svo_msgs__msg__DenseInputWithFeatures__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!svo_msgs__msg__DenseInputWithFeatures__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
svo_msgs__msg__DenseInputWithFeatures__Sequence__copy(
  const svo_msgs__msg__DenseInputWithFeatures__Sequence * input,
  svo_msgs__msg__DenseInputWithFeatures__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(svo_msgs__msg__DenseInputWithFeatures);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    svo_msgs__msg__DenseInputWithFeatures * data =
      (svo_msgs__msg__DenseInputWithFeatures *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!svo_msgs__msg__DenseInputWithFeatures__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          svo_msgs__msg__DenseInputWithFeatures__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!svo_msgs__msg__DenseInputWithFeatures__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
