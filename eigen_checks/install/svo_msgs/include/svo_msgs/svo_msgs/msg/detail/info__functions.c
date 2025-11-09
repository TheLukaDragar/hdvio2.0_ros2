// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from svo_msgs:msg/Info.idl
// generated code does not contain a copyright notice
#include "svo_msgs/msg/detail/info__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `header`
#include "std_msgs/msg/detail/header__functions.h"
// Member `keyframes`
#include "rosidl_runtime_c/primitives_sequence_functions.h"

bool
svo_msgs__msg__Info__init(svo_msgs__msg__Info * msg)
{
  if (!msg) {
    return false;
  }
  // header
  if (!std_msgs__msg__Header__init(&msg->header)) {
    svo_msgs__msg__Info__fini(msg);
    return false;
  }
  // processing_time
  // keyframes
  if (!rosidl_runtime_c__int32__Sequence__init(&msg->keyframes, 0)) {
    svo_msgs__msg__Info__fini(msg);
    return false;
  }
  // num_matches
  // tracking_quality
  // stage
  return true;
}

void
svo_msgs__msg__Info__fini(svo_msgs__msg__Info * msg)
{
  if (!msg) {
    return;
  }
  // header
  std_msgs__msg__Header__fini(&msg->header);
  // processing_time
  // keyframes
  rosidl_runtime_c__int32__Sequence__fini(&msg->keyframes);
  // num_matches
  // tracking_quality
  // stage
}

bool
svo_msgs__msg__Info__are_equal(const svo_msgs__msg__Info * lhs, const svo_msgs__msg__Info * rhs)
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
  // processing_time
  if (lhs->processing_time != rhs->processing_time) {
    return false;
  }
  // keyframes
  if (!rosidl_runtime_c__int32__Sequence__are_equal(
      &(lhs->keyframes), &(rhs->keyframes)))
  {
    return false;
  }
  // num_matches
  if (lhs->num_matches != rhs->num_matches) {
    return false;
  }
  // tracking_quality
  if (lhs->tracking_quality != rhs->tracking_quality) {
    return false;
  }
  // stage
  if (lhs->stage != rhs->stage) {
    return false;
  }
  return true;
}

bool
svo_msgs__msg__Info__copy(
  const svo_msgs__msg__Info * input,
  svo_msgs__msg__Info * output)
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
  // processing_time
  output->processing_time = input->processing_time;
  // keyframes
  if (!rosidl_runtime_c__int32__Sequence__copy(
      &(input->keyframes), &(output->keyframes)))
  {
    return false;
  }
  // num_matches
  output->num_matches = input->num_matches;
  // tracking_quality
  output->tracking_quality = input->tracking_quality;
  // stage
  output->stage = input->stage;
  return true;
}

svo_msgs__msg__Info *
svo_msgs__msg__Info__create()
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  svo_msgs__msg__Info * msg = (svo_msgs__msg__Info *)allocator.allocate(sizeof(svo_msgs__msg__Info), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(svo_msgs__msg__Info));
  bool success = svo_msgs__msg__Info__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
svo_msgs__msg__Info__destroy(svo_msgs__msg__Info * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    svo_msgs__msg__Info__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
svo_msgs__msg__Info__Sequence__init(svo_msgs__msg__Info__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  svo_msgs__msg__Info * data = NULL;

  if (size) {
    data = (svo_msgs__msg__Info *)allocator.zero_allocate(size, sizeof(svo_msgs__msg__Info), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = svo_msgs__msg__Info__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        svo_msgs__msg__Info__fini(&data[i - 1]);
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
svo_msgs__msg__Info__Sequence__fini(svo_msgs__msg__Info__Sequence * array)
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
      svo_msgs__msg__Info__fini(&array->data[i]);
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

svo_msgs__msg__Info__Sequence *
svo_msgs__msg__Info__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  svo_msgs__msg__Info__Sequence * array = (svo_msgs__msg__Info__Sequence *)allocator.allocate(sizeof(svo_msgs__msg__Info__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = svo_msgs__msg__Info__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
svo_msgs__msg__Info__Sequence__destroy(svo_msgs__msg__Info__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    svo_msgs__msg__Info__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
svo_msgs__msg__Info__Sequence__are_equal(const svo_msgs__msg__Info__Sequence * lhs, const svo_msgs__msg__Info__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!svo_msgs__msg__Info__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
svo_msgs__msg__Info__Sequence__copy(
  const svo_msgs__msg__Info__Sequence * input,
  svo_msgs__msg__Info__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(svo_msgs__msg__Info);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    svo_msgs__msg__Info * data =
      (svo_msgs__msg__Info *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!svo_msgs__msg__Info__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          svo_msgs__msg__Info__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!svo_msgs__msg__Info__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
