// generated from rosidl_generator_py/resource/_idl_support.c.em
// with input from svo_msgs:msg/Command.idl
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
#include "svo_msgs/msg/detail/command__struct.h"
#include "svo_msgs/msg/detail/command__functions.h"

#include "rosidl_runtime_c/primitives_sequence.h"
#include "rosidl_runtime_c/primitives_sequence_functions.h"

ROSIDL_GENERATOR_C_IMPORT
bool std_msgs__msg__header__convert_from_py(PyObject * _pymsg, void * _ros_message);
ROSIDL_GENERATOR_C_IMPORT
PyObject * std_msgs__msg__header__convert_to_py(void * raw_ros_message);
ROSIDL_GENERATOR_C_IMPORT
bool geometry_msgs__msg__vector3__convert_from_py(PyObject * _pymsg, void * _ros_message);
ROSIDL_GENERATOR_C_IMPORT
PyObject * geometry_msgs__msg__vector3__convert_to_py(void * raw_ros_message);

ROSIDL_GENERATOR_C_EXPORT
bool svo_msgs__msg__command__convert_from_py(PyObject * _pymsg, void * _ros_message)
{
  // check that the passed message is of the expected Python class
  {
    char full_classname_dest[30];
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
    assert(strncmp("svo_msgs.msg._command.Command", full_classname_dest, 29) == 0);
  }
  svo_msgs__msg__Command * ros_message = _ros_message;
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
  {  // t
    PyObject * field = PyObject_GetAttrString(_pymsg, "t");
    if (!field) {
      return false;
    }
    assert(PyFloat_Check(field));
    ros_message->t = PyFloat_AS_DOUBLE(field);
    Py_DECREF(field);
  }
  {  // is_single_rotor_thrust
    PyObject * field = PyObject_GetAttrString(_pymsg, "is_single_rotor_thrust");
    if (!field) {
      return false;
    }
    assert(PyBool_Check(field));
    ros_message->is_single_rotor_thrust = (Py_True == field);
    Py_DECREF(field);
  }
  {  // collective_thrust
    PyObject * field = PyObject_GetAttrString(_pymsg, "collective_thrust");
    if (!field) {
      return false;
    }
    assert(PyFloat_Check(field));
    ros_message->collective_thrust = PyFloat_AS_DOUBLE(field);
    Py_DECREF(field);
  }
  {  // bodyrates
    PyObject * field = PyObject_GetAttrString(_pymsg, "bodyrates");
    if (!field) {
      return false;
    }
    if (!geometry_msgs__msg__vector3__convert_from_py(field, &ros_message->bodyrates)) {
      Py_DECREF(field);
      return false;
    }
    Py_DECREF(field);
  }
  {  // thrusts
    PyObject * field = PyObject_GetAttrString(_pymsg, "thrusts");
    if (!field) {
      return false;
    }
    {
      // TODO(dirk-thomas) use a better way to check the type before casting
      assert(field->ob_type != NULL);
      assert(field->ob_type->tp_name != NULL);
      assert(strcmp(field->ob_type->tp_name, "numpy.ndarray") == 0);
      PyArrayObject * seq_field = (PyArrayObject *)field;
      Py_INCREF(seq_field);
      assert(PyArray_NDIM(seq_field) == 1);
      assert(PyArray_TYPE(seq_field) == NPY_FLOAT64);
      Py_ssize_t size = 4;
      double * dest = ros_message->thrusts;
      for (Py_ssize_t i = 0; i < size; ++i) {
        double tmp = *(npy_float64 *)PyArray_GETPTR1(seq_field, i);
        memcpy(&dest[i], &tmp, sizeof(double));
      }
      Py_DECREF(seq_field);
    }
    Py_DECREF(field);
  }

  return true;
}

ROSIDL_GENERATOR_C_EXPORT
PyObject * svo_msgs__msg__command__convert_to_py(void * raw_ros_message)
{
  /* NOTE(esteve): Call constructor of Command */
  PyObject * _pymessage = NULL;
  {
    PyObject * pymessage_module = PyImport_ImportModule("svo_msgs.msg._command");
    assert(pymessage_module);
    PyObject * pymessage_class = PyObject_GetAttrString(pymessage_module, "Command");
    assert(pymessage_class);
    Py_DECREF(pymessage_module);
    _pymessage = PyObject_CallObject(pymessage_class, NULL);
    Py_DECREF(pymessage_class);
    if (!_pymessage) {
      return NULL;
    }
  }
  svo_msgs__msg__Command * ros_message = (svo_msgs__msg__Command *)raw_ros_message;
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
  {  // t
    PyObject * field = NULL;
    field = PyFloat_FromDouble(ros_message->t);
    {
      int rc = PyObject_SetAttrString(_pymessage, "t", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // is_single_rotor_thrust
    PyObject * field = NULL;
    field = PyBool_FromLong(ros_message->is_single_rotor_thrust ? 1 : 0);
    {
      int rc = PyObject_SetAttrString(_pymessage, "is_single_rotor_thrust", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // collective_thrust
    PyObject * field = NULL;
    field = PyFloat_FromDouble(ros_message->collective_thrust);
    {
      int rc = PyObject_SetAttrString(_pymessage, "collective_thrust", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // bodyrates
    PyObject * field = NULL;
    field = geometry_msgs__msg__vector3__convert_to_py(&ros_message->bodyrates);
    if (!field) {
      return NULL;
    }
    {
      int rc = PyObject_SetAttrString(_pymessage, "bodyrates", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // thrusts
    PyObject * field = NULL;
    field = PyObject_GetAttrString(_pymessage, "thrusts");
    if (!field) {
      return NULL;
    }
    assert(field->ob_type != NULL);
    assert(field->ob_type->tp_name != NULL);
    assert(strcmp(field->ob_type->tp_name, "numpy.ndarray") == 0);
    PyArrayObject * seq_field = (PyArrayObject *)field;
    assert(PyArray_NDIM(seq_field) == 1);
    assert(PyArray_TYPE(seq_field) == NPY_FLOAT64);
    assert(sizeof(npy_float64) == sizeof(double));
    npy_float64 * dst = (npy_float64 *)PyArray_GETPTR1(seq_field, 0);
    double * src = &(ros_message->thrusts[0]);
    memcpy(dst, src, 4 * sizeof(double));
    Py_DECREF(field);
  }

  // ownership of _pymessage is transferred to the caller
  return _pymessage;
}
