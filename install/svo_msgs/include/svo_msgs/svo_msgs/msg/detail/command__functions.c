// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from svo_msgs:msg/Command.idl
// generated code does not contain a copyright notice
#include "svo_msgs/msg/detail/command__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `header`
#include "std_msgs/msg/detail/header__functions.h"
// Member `bodyrates`
#include "geometry_msgs/msg/detail/vector3__functions.h"

bool
svo_msgs__msg__Command__init(svo_msgs__msg__Command * msg)
{
  if (!msg) {
    return false;
  }
  // header
  if (!std_msgs__msg__Header__init(&msg->header)) {
    svo_msgs__msg__Command__fini(msg);
    return false;
  }
  // t
  // is_single_rotor_thrust
  // collective_thrust
  // bodyrates
  if (!geometry_msgs__msg__Vector3__init(&msg->bodyrates)) {
    svo_msgs__msg__Command__fini(msg);
    return false;
  }
  // thrusts
  return true;
}

void
svo_msgs__msg__Command__fini(svo_msgs__msg__Command * msg)
{
  if (!msg) {
    return;
  }
  // header
  std_msgs__msg__Header__fini(&msg->header);
  // t
  // is_single_rotor_thrust
  // collective_thrust
  // bodyrates
  geometry_msgs__msg__Vector3__fini(&msg->bodyrates);
  // thrusts
}

bool
svo_msgs__msg__Command__are_equal(const svo_msgs__msg__Command * lhs, const svo_msgs__msg__Command * rhs)
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
  // t
  if (lhs->t != rhs->t) {
    return false;
  }
  // is_single_rotor_thrust
  if (lhs->is_single_rotor_thrust != rhs->is_single_rotor_thrust) {
    return false;
  }
  // collective_thrust
  if (lhs->collective_thrust != rhs->collective_thrust) {
    return false;
  }
  // bodyrates
  if (!geometry_msgs__msg__Vector3__are_equal(
      &(lhs->bodyrates), &(rhs->bodyrates)))
  {
    return false;
  }
  // thrusts
  for (size_t i = 0; i < 4; ++i) {
    if (lhs->thrusts[i] != rhs->thrusts[i]) {
      return false;
    }
  }
  return true;
}

bool
svo_msgs__msg__Command__copy(
  const svo_msgs__msg__Command * input,
  svo_msgs__msg__Command * output)
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
  // t
  output->t = input->t;
  // is_single_rotor_thrust
  output->is_single_rotor_thrust = input->is_single_rotor_thrust;
  // collective_thrust
  output->collective_thrust = input->collective_thrust;
  // bodyrates
  if (!geometry_msgs__msg__Vector3__copy(
      &(input->bodyrates), &(output->bodyrates)))
  {
    return false;
  }
  // thrusts
  for (size_t i = 0; i < 4; ++i) {
    output->thrusts[i] = input->thrusts[i];
  }
  return true;
}

svo_msgs__msg__Command *
svo_msgs__msg__Command__create()
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  svo_msgs__msg__Command * msg = (svo_msgs__msg__Command *)allocator.allocate(sizeof(svo_msgs__msg__Command), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(svo_msgs__msg__Command));
  bool success = svo_msgs__msg__Command__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
svo_msgs__msg__Command__destroy(svo_msgs__msg__Command * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    svo_msgs__msg__Command__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
svo_msgs__msg__Command__Sequence__init(svo_msgs__msg__Command__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  svo_msgs__msg__Command * data = NULL;

  if (size) {
    data = (svo_msgs__msg__Command *)allocator.zero_allocate(size, sizeof(svo_msgs__msg__Command), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = svo_msgs__msg__Command__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        svo_msgs__msg__Command__fini(&data[i - 1]);
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
svo_msgs__msg__Command__Sequence__fini(svo_msgs__msg__Command__Sequence * array)
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
      svo_msgs__msg__Command__fini(&array->data[i]);
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

svo_msgs__msg__Command__Sequence *
svo_msgs__msg__Command__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  svo_msgs__msg__Command__Sequence * array = (svo_msgs__msg__Command__Sequence *)allocator.allocate(sizeof(svo_msgs__msg__Command__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = svo_msgs__msg__Command__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
svo_msgs__msg__Command__Sequence__destroy(svo_msgs__msg__Command__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    svo_msgs__msg__Command__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
svo_msgs__msg__Command__Sequence__are_equal(const svo_msgs__msg__Command__Sequence * lhs, const svo_msgs__msg__Command__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!svo_msgs__msg__Command__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
svo_msgs__msg__Command__Sequence__copy(
  const svo_msgs__msg__Command__Sequence * input,
  svo_msgs__msg__Command__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(svo_msgs__msg__Command);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    svo_msgs__msg__Command * data =
      (svo_msgs__msg__Command *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!svo_msgs__msg__Command__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          svo_msgs__msg__Command__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!svo_msgs__msg__Command__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
