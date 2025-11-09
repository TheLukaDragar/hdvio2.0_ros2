// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from svo_msgs:msg/DenseInput.idl
// generated code does not contain a copyright notice
#include "svo_msgs/msg/detail/dense_input__functions.h"

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

bool
svo_msgs__msg__DenseInput__init(svo_msgs__msg__DenseInput * msg)
{
  if (!msg) {
    return false;
  }
  // header
  if (!std_msgs__msg__Header__init(&msg->header)) {
    svo_msgs__msg__DenseInput__fini(msg);
    return false;
  }
  // frame_id
  // pose
  if (!geometry_msgs__msg__Pose__init(&msg->pose)) {
    svo_msgs__msg__DenseInput__fini(msg);
    return false;
  }
  // image
  if (!sensor_msgs__msg__Image__init(&msg->image)) {
    svo_msgs__msg__DenseInput__fini(msg);
    return false;
  }
  // min_depth
  // max_depth
  return true;
}

void
svo_msgs__msg__DenseInput__fini(svo_msgs__msg__DenseInput * msg)
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
}

bool
svo_msgs__msg__DenseInput__are_equal(const svo_msgs__msg__DenseInput * lhs, const svo_msgs__msg__DenseInput * rhs)
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
  return true;
}

bool
svo_msgs__msg__DenseInput__copy(
  const svo_msgs__msg__DenseInput * input,
  svo_msgs__msg__DenseInput * output)
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
  return true;
}

svo_msgs__msg__DenseInput *
svo_msgs__msg__DenseInput__create()
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  svo_msgs__msg__DenseInput * msg = (svo_msgs__msg__DenseInput *)allocator.allocate(sizeof(svo_msgs__msg__DenseInput), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(svo_msgs__msg__DenseInput));
  bool success = svo_msgs__msg__DenseInput__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
svo_msgs__msg__DenseInput__destroy(svo_msgs__msg__DenseInput * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    svo_msgs__msg__DenseInput__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
svo_msgs__msg__DenseInput__Sequence__init(svo_msgs__msg__DenseInput__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  svo_msgs__msg__DenseInput * data = NULL;

  if (size) {
    data = (svo_msgs__msg__DenseInput *)allocator.zero_allocate(size, sizeof(svo_msgs__msg__DenseInput), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = svo_msgs__msg__DenseInput__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        svo_msgs__msg__DenseInput__fini(&data[i - 1]);
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
svo_msgs__msg__DenseInput__Sequence__fini(svo_msgs__msg__DenseInput__Sequence * array)
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
      svo_msgs__msg__DenseInput__fini(&array->data[i]);
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

svo_msgs__msg__DenseInput__Sequence *
svo_msgs__msg__DenseInput__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  svo_msgs__msg__DenseInput__Sequence * array = (svo_msgs__msg__DenseInput__Sequence *)allocator.allocate(sizeof(svo_msgs__msg__DenseInput__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = svo_msgs__msg__DenseInput__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
svo_msgs__msg__DenseInput__Sequence__destroy(svo_msgs__msg__DenseInput__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    svo_msgs__msg__DenseInput__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
svo_msgs__msg__DenseInput__Sequence__are_equal(const svo_msgs__msg__DenseInput__Sequence * lhs, const svo_msgs__msg__DenseInput__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!svo_msgs__msg__DenseInput__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
svo_msgs__msg__DenseInput__Sequence__copy(
  const svo_msgs__msg__DenseInput__Sequence * input,
  svo_msgs__msg__DenseInput__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(svo_msgs__msg__DenseInput);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    svo_msgs__msg__DenseInput * data =
      (svo_msgs__msg__DenseInput *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!svo_msgs__msg__DenseInput__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          svo_msgs__msg__DenseInput__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!svo_msgs__msg__DenseInput__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
