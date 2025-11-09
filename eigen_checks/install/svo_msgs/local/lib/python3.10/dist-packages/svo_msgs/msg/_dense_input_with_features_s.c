// generated from rosidl_generator_py/resource/_idl_support.c.em
// with input from svo_msgs:msg/DenseInputWithFeatures.idl
// generated code does not contain a copyright notice
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <Python.h>
#include <stdbool.h>
#ifndef _WIN32
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-function"
#endif
#include "numpy/ndarrayobject.h"
#ifndef _WIN32
# pragma GCC diagnostic pop
#endif
#include "rosidl_runtime_c/visibility_control.h"
#include "svo_msgs/msg/detail/dense_input_with_features__struct.h"
#include "svo_msgs/msg/detail/dense_input_with_features__functions.h"

#include "rosidl_runtime_c/primitives_sequence.h"
#include "rosidl_runtime_c/primitives_sequence_functions.h"

// Nested array functions includes
#include "svo_msgs/msg/detail/feature__functions.h"
// end nested array functions include
ROSIDL_GENERATOR_C_IMPORT
bool std_msgs__msg__header__convert_from_py(PyObject * _pymsg, void * _ros_message);
ROSIDL_GENERATOR_C_IMPORT
PyObject * std_msgs__msg__header__convert_to_py(void * raw_ros_message);
ROSIDL_GENERATOR_C_IMPORT
bool geometry_msgs__msg__pose__convert_from_py(PyObject * _pymsg, void * _ros_message);
ROSIDL_GENERATOR_C_IMPORT
PyObject * geometry_msgs__msg__pose__convert_to_py(void * raw_ros_message);
ROSIDL_GENERATOR_C_IMPORT
bool sensor_msgs__msg__image__convert_from_py(PyObject * _pymsg, void * _ros_message);
ROSIDL_GENERATOR_C_IMPORT
PyObject * sensor_msgs__msg__image__convert_to_py(void * raw_ros_message);
bool svo_msgs__msg__feature__convert_from_py(PyObject * _pymsg, void * _ros_message);
PyObject * svo_msgs__msg__feature__convert_to_py(void * raw_ros_message);

ROSIDL_GENERATOR_C_EXPORT
bool svo_msgs__msg__dense_input_with_features__convert_from_py(PyObject * _pymsg, void * _ros_message)
{
  // check that the passed message is of the expected Python class
  {
    char full_classname_dest[63];
    {
      char * class_name = NULL;
      char * module_name = NULL;
      {
        PyObject * class_attr = PyObject_GetAttrString(_pymsg, "__class__");
        if (class_attr) {
          PyObject * name_attr = PyObject_GetAttrString(class_attr, "__name__");
          if (name_attr) {
            class_name = (char *)PyUnicode_1BYTE_DATA(name_attr);
            Py_DECREF(name_attr);
          }
          PyObject * module_attr = PyObject_GetAttrString(class_attr, "__module__");
          if (module_attr) {
            module_name = (char *)PyUnicode_1BYTE_DATA(module_attr);
            Py_DECREF(module_attr);
          }
          Py_DECREF(class_attr);
        }
      }
      if (!class_name || !module_name) {
        return false;
      }
      snprintf(full_classname_dest, sizeof(full_classname_dest), "%s.%s", module_name, class_name);
    }
    assert(strncmp("svo_msgs.msg._dense_input_with_features.DenseInputWithFeatures", full_classname_dest, 62) == 0);
  }
  svo_msgs__msg__DenseInputWithFeatures * ros_message = _ros_message;
  {  // header
    PyObject * field = PyObject_GetAttrString(_pymsg, "header");
    if (!field) {
      return false;
    }
    if (!std_msgs__msg__header__convert_from_py(field, &ros_message->header)) {
      Py_DECREF(field);
      return false;
    }
    Py_DECREF(field);
  }
  {  // frame_id
    PyObject * field = PyObject_GetAttrString(_pymsg, "frame_id");
    if (!field) {
      return false;
    }
    assert(PyLong_Check(field));
    ros_message->frame_id = PyLong_AsUnsignedLong(field);
    Py_DECREF(field);
  }
  {  // pose
    PyObject * field = PyObject_GetAttrString(_pymsg, "pose");
    if (!field) {
      return false;
    }
    if (!geometry_msgs__msg__pose__convert_from_py(field, &ros_message->pose)) {
      Py_DECREF(field);
      return false;
    }
    Py_DECREF(field);
  }
  {  // image
    PyObject * field = PyObject_GetAttrString(_pymsg, "image");
    if (!field) {
      return false;
    }
    if (!sensor_msgs__msg__image__convert_from_py(field, &ros_message->image)) {
      Py_DECREF(field);
      return false;
    }
    Py_DECREF(field);
  }
  {  // min_depth
    PyObject * field = PyObject_GetAttrString(_pymsg, "min_depth");
    if (!field) {
      return false;
    }
    assert(PyFloat_Check(field));
    ros_message->min_depth = (float)PyFloat_AS_DOUBLE(field);
    Py_DECREF(field);
  }
  {  // max_depth
    PyObject * field = PyObject_GetAttrString(_pymsg, "max_depth");
    if (!field) {
      return false;
    }
    assert(PyFloat_Check(field));
    ros_message->max_depth = (float)PyFloat_AS_DOUBLE(field);
    Py_DECREF(field);
  }
  {  // features
    PyObject * field = PyObject_GetAttrString(_pymsg, "features");
    if (!field) {
      return false;
    }
    PyObject * seq_field = PySequence_Fast(field, "expected a sequence in 'features'");
    if (!seq_field) {
      Py_DECREF(field);
      return false;
    }
    Py_ssize_t size = PySequence_Size(field);
    if (-1 == size) {
      Py_DECREF(seq_field);
      Py_DECREF(field);
      return false;
    }
    if (!svo_msgs__msg__Feature__Sequence__init(&(ros_message->features), size)) {
      PyErr_SetString(PyExc_RuntimeError, "unable to create svo_msgs__msg__Feature__Sequence ros_message");
      Py_DECREF(seq_field);
      Py_DECREF(field);
      return false;
    }
    svo_msgs__msg__Feature * dest = ros_message->features.data;
    for (Py_ssize_t i = 0; i < size; ++i) {
      if (!svo_msgs__msg__feature__convert_from_py(PySequence_Fast_GET_ITEM(seq_field, i), &dest[i])) {
        Py_DECREF(seq_field);
        Py_DECREF(field);
        return false;
      }
    }
    Py_DECREF(seq_field);
    Py_DECREF(field);
  }

  return true;
}

ROSIDL_GENERATOR_C_EXPORT
PyObject * svo_msgs__msg__dense_input_with_features__convert_to_py(void * raw_ros_message)
{
  /* NOTE(esteve): Call constructor of DenseInputWithFeatures */
  PyObject * _pymessage = NULL;
  {
    PyObject * pymessage_module = PyImport_ImportModule("svo_msgs.msg._dense_input_with_features");
    assert(pymessage_module);
    PyObject * pymessage_class = PyObject_GetAttrString(pymessage_module, "DenseInputWithFeatures");
    assert(pymessage_class);
    Py_DECREF(pymessage_module);
    _pymessage = PyObject_CallObject(pymessage_class, NULL);
    Py_DECREF(pymessage_class);
    if (!_pymessage) {
      return NULL;
    }
  }
  svo_msgs__msg__DenseInputWithFeatures * ros_message = (svo_msgs__msg__DenseInputWithFeatures *)raw_ros_message;
  {  // header
    PyObject * field = NULL;
    field = std_msgs__msg__header__convert_to_py(&ros_message->header);
    if (!field) {
      return NULL;
    }
    {
      int rc = PyObject_SetAttrString(_pymessage, "header", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // frame_id
    PyObject * field = NULL;
    field = PyLong_FromUnsignedLong(ros_message->frame_id);
    {
      int rc = PyObject_SetAttrString(_pymessage, "frame_id", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // pose
    PyObject * field = NULL;
    field = geometry_msgs__msg__pose__convert_to_py(&ros_message->pose);
    if (!field) {
      return NULL;
    }
    {
      int rc = PyObject_SetAttrString(_pymessage, "pose", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // image
    PyObject * field = NULL;
    field = sensor_msgs__msg__image__convert_to_py(&ros_message->image);
    if (!field) {
      return NULL;
    }
    {
      int rc = PyObject_SetAttrString(_pymessage, "image", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // min_depth
    PyObject * field = NULL;
    field = PyFloat_FromDouble(ros_message->min_depth);
    {
      int rc = PyObject_SetAttrString(_pymessage, "min_depth", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // max_depth
    PyObject * field = NULL;
    field = PyFloat_FromDouble(ros_message->max_depth);
    {
      int rc = PyObject_SetAttrString(_pymessage, "max_depth", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // features
    PyObject * field = NULL;
    size_t size = ros_message->features.size;
    field = PyList_New(size);
    if (!field) {
      return NULL;
    }
    svo_msgs__msg__Feature * item;
    for (size_t i = 0; i < size; ++i) {
      item = &(ros_message->features.data[i]);
      PyObject * pyitem = svo_msgs__msg__feature__convert_to_py(item);
      if (!pyitem) {
        Py_DECREF(field);
        return NULL;
      }
      int rc = PyList_SetItem(field, i, pyitem);
      (void)rc;
      assert(rc == 0);
    }
    assert(PySequence_Check(field));
    {
      int rc = PyObject_SetAttrString(_pymessage, "features", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }

  // ownership of _pymessage is transferred to the caller
  return _pymessage;
}
