# generated from rosidl_generator_py/resource/_idl.py.em
# with input from svo_msgs:msg/Command.idl
# generated code does not contain a copyright notice


# Import statements for member types

import builtins  # noqa: E402, I100

import math  # noqa: E402, I100

# Member 'thrusts'
import numpy  # noqa: E402, I100

import rosidl_parser.definition  # noqa: E402, I100


class Metaclass_Command(type):
    """Metaclass of message 'Command'."""

    _CREATE_ROS_MESSAGE = None
    _CONVERT_FROM_PY = None
    _CONVERT_TO_PY = None
    _DESTROY_ROS_MESSAGE = None
    _TYPE_SUPPORT = None

    __constants = {
    }

    @classmethod
    def __import_type_support__(cls):
        try:
            from rosidl_generator_py import import_type_support
            module = import_type_support('svo_msgs')
        except ImportError:
            import logging
            import traceback
            logger = logging.getLogger(
                'svo_msgs.msg.Command')
            logger.debug(
                'Failed to import needed modules for type support:\n' +
                traceback.format_exc())
        else:
            cls._CREATE_ROS_MESSAGE = module.create_ros_message_msg__msg__command
            cls._CONVERT_FROM_PY = module.convert_from_py_msg__msg__command
            cls._CONVERT_TO_PY = module.convert_to_py_msg__msg__command
            cls._TYPE_SUPPORT = module.type_support_msg__msg__command
            cls._DESTROY_ROS_MESSAGE = module.destroy_ros_message_msg__msg__command

            from geometry_msgs.msg import Vector3
            if Vector3.__class__._TYPE_SUPPORT is None:
                Vector3.__class__.__import_type_support__()

            from std_msgs.msg import Header
            if Header.__class__._TYPE_SUPPORT is None:
                Header.__class__.__import_type_support__()

    @classmethod
    def __prepare__(cls, name, bases, **kwargs):
        # list constant names here so that they appear in the help text of
        # the message class under "Data and other attributes defined here:"
        # as well as populate each message instance
        return {
        }


class Command(metaclass=Metaclass_Command):
    """Message class 'Command'."""

    __slots__ = [
        '_header',
        '_t',
        '_is_single_rotor_thrust',
        '_collective_thrust',
        '_bodyrates',
        '_thrusts',
    ]

    _fields_and_field_types = {
        'header': 'std_msgs/Header',
        't': 'double',
        'is_single_rotor_thrust': 'boolean',
        'collective_thrust': 'double',
        'bodyrates': 'geometry_msgs/Vector3',
        'thrusts': 'double[4]',
    }

    SLOT_TYPES = (
        rosidl_parser.definition.NamespacedType(['std_msgs', 'msg'], 'Header'),  # noqa: E501
        rosidl_parser.definition.BasicType('double'),  # noqa: E501
        rosidl_parser.definition.BasicType('boolean'),  # noqa: E501
        rosidl_parser.definition.BasicType('double'),  # noqa: E501
        rosidl_parser.definition.NamespacedType(['geometry_msgs', 'msg'], 'Vector3'),  # noqa: E501
        rosidl_parser.definition.Array(rosidl_parser.definition.BasicType('double'), 4),  # noqa: E501
    )

    def __init__(self, **kwargs):
        assert all('_' + key in self.__slots__ for key in kwargs.keys()), \
            'Invalid arguments passed to constructor: %s' % \
            ', '.join(sorted(k for k in kwargs.keys() if '_' + k not in self.__slots__))
        from std_msgs.msg import Header
        self.header = kwargs.get('header', Header())
        self.t = kwargs.get('t', float())
        self.is_single_rotor_thrust = kwargs.get('is_single_rotor_thrust', bool())
        self.collective_thrust = kwargs.get('collective_thrust', float())
        from geometry_msgs.msg import Vector3
        self.bodyrates = kwargs.get('bodyrates', Vector3())
        if 'thrusts' not in kwargs:
            self.thrusts = numpy.zeros(4, dtype=numpy.float64)
        else:
            self.thrusts = numpy.array(kwargs.get('thrusts'), dtype=numpy.float64)
            assert self.thrusts.shape == (4, )

    def __repr__(self):
        typename = self.__class__.__module__.split('.')
        typename.pop()
        typename.append(self.__class__.__name__)
        args = []
        for s, t in zip(self.__slots__, self.SLOT_TYPES):
            field = getattr(self, s)
            fieldstr = repr(field)
            # We use Python array type for fields that can be directly stored
            # in them, and "normal" sequences for everything else.  If it is
            # a type that we store in an array, strip off the 'array' portion.
            if (
                isinstance(t, rosidl_parser.definition.AbstractSequence) and
                isinstance(t.value_type, rosidl_parser.definition.BasicType) and
                t.value_type.typename in ['float', 'double', 'int8', 'uint8', 'int16', 'uint16', 'int32', 'uint32', 'int64', 'uint64']
            ):
                if len(field) == 0:
                    fieldstr = '[]'
                else:
                    assert fieldstr.startswith('array(')
                    prefix = "array('X', "
                    suffix = ')'
                    fieldstr = fieldstr[len(prefix):-len(suffix)]
            args.append(s[1:] + '=' + fieldstr)
        return '%s(%s)' % ('.'.join(typename), ', '.join(args))

    def __eq__(self, other):
        if not isinstance(other, self.__class__):
            return False
        if self.header != other.header:
            return False
        if self.t != other.t:
            return False
        if self.is_single_rotor_thrust != other.is_single_rotor_thrust:
            return False
        if self.collective_thrust != other.collective_thrust:
            return False
        if self.bodyrates != other.bodyrates:
            return False
        if any(self.thrusts != other.thrusts):
            return False
        return True

    @classmethod
    def get_fields_and_field_types(cls):
        from copy import copy
        return copy(cls._fields_and_field_types)

    @builtins.property
    def header(self):
        """Message field 'header'."""
        return self._header

    @header.setter
    def header(self, value):
        if __debug__:
            from std_msgs.msg import Header
            assert \
                isinstance(value, Header), \
                "The 'header' field must be a sub message of type 'Header'"
        self._header = value

    @builtins.property
    def t(self):
        """Message field 't'."""
        return self._t

    @t.setter
    def t(self, value):
        if __debug__:
            assert \
                isinstance(value, float), \
                "The 't' field must be of type 'float'"
            assert not (value < -1.7976931348623157e+308 or value > 1.7976931348623157e+308) or math.isinf(value), \
                "The 't' field must be a double in [-1.7976931348623157e+308, 1.7976931348623157e+308]"
        self._t = value

    @builtins.property
    def is_single_rotor_thrust(self):
        """Message field 'is_single_rotor_thrust'."""
        return self._is_single_rotor_thrust

    @is_single_rotor_thrust.setter
    def is_single_rotor_thrust(self, value):
        if __debug__:
            assert \
                isinstance(value, bool), \
                "The 'is_single_rotor_thrust' field must be of type 'bool'"
        self._is_single_rotor_thrust = value

    @builtins.property
    def collective_thrust(self):
        """Message field 'collective_thrust'."""
        return self._collective_thrust

    @collective_thrust.setter
    def collective_thrust(self, value):
        if __debug__:
            assert \
                isinstance(value, float), \
                "The 'collective_thrust' field must be of type 'float'"
            assert not (value < -1.7976931348623157e+308 or value > 1.7976931348623157e+308) or math.isinf(value), \
                "The 'collective_thrust' field must be a double in [-1.7976931348623157e+308, 1.7976931348623157e+308]"
        self._collective_thrust = value

    @builtins.property
    def bodyrates(self):
        """Message field 'bodyrates'."""
        return self._bodyrates

    @bodyrates.setter
    def bodyrates(self, value):
        if __debug__:
            from geometry_msgs.msg import Vector3
            assert \
                isinstance(value, Vector3), \
                "The 'bodyrates' field must be a sub message of type 'Vector3'"
        self._bodyrates = value

    @builtins.property
    def thrusts(self):
        """Message field 'thrusts'."""
        return self._thrusts

    @thrusts.setter
    def thrusts(self, value):
        if isinstance(value, numpy.ndarray):
            assert value.dtype == numpy.float64, \
                "The 'thrusts' numpy.ndarray() must have the dtype of 'numpy.float64'"
            assert value.size == 4, \
                "The 'thrusts' numpy.ndarray() must have a size of 4"
            self._thrusts = value
            return
        if __debug__:
            from collections.abc import Sequence
            from collections.abc import Set
            from collections import UserList
            from collections import UserString
            assert \
                ((isinstance(value, Sequence) or
                  isinstance(value, Set) or
                  isinstance(value, UserList)) and
                 not isinstance(value, str) and
                 not isinstance(value, UserString) and
                 len(value) == 4 and
                 all(isinstance(v, float) for v in value) and
                 all(not (val < -1.7976931348623157e+308 or val > 1.7976931348623157e+308) or math.isinf(val) for val in value)), \
                "The 'thrusts' field must be a set or sequence with length 4 and each value of type 'float' and each double in [-179769313486231570814527423731704356798070567525844996598917476803157260780028538760589558632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245490090389328944075868508455133942304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368.000000, 179769313486231570814527423731704356798070567525844996598917476803157260780028538760589558632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245490090389328944075868508455133942304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368.000000]"
        self._thrusts = numpy.array(value, dtype=numpy.float64)
