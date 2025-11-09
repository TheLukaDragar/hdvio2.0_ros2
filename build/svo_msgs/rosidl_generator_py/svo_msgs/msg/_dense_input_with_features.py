# generated from rosidl_generator_py/resource/_idl.py.em
# with input from svo_msgs:msg/DenseInputWithFeatures.idl
# generated code does not contain a copyright notice


# Import statements for member types

import builtins  # noqa: E402, I100

import math  # noqa: E402, I100

import rosidl_parser.definition  # noqa: E402, I100


class Metaclass_DenseInputWithFeatures(type):
    """Metaclass of message 'DenseInputWithFeatures'."""

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
                'svo_msgs.msg.DenseInputWithFeatures')
            logger.debug(
                'Failed to import needed modules for type support:\n' +
                traceback.format_exc())
        else:
            cls._CREATE_ROS_MESSAGE = module.create_ros_message_msg__msg__dense_input_with_features
            cls._CONVERT_FROM_PY = module.convert_from_py_msg__msg__dense_input_with_features
            cls._CONVERT_TO_PY = module.convert_to_py_msg__msg__dense_input_with_features
            cls._TYPE_SUPPORT = module.type_support_msg__msg__dense_input_with_features
            cls._DESTROY_ROS_MESSAGE = module.destroy_ros_message_msg__msg__dense_input_with_features

            from geometry_msgs.msg import Pose
            if Pose.__class__._TYPE_SUPPORT is None:
                Pose.__class__.__import_type_support__()

            from sensor_msgs.msg import Image
            if Image.__class__._TYPE_SUPPORT is None:
                Image.__class__.__import_type_support__()

            from std_msgs.msg import Header
            if Header.__class__._TYPE_SUPPORT is None:
                Header.__class__.__import_type_support__()

            from svo_msgs.msg import Feature
            if Feature.__class__._TYPE_SUPPORT is None:
                Feature.__class__.__import_type_support__()

    @classmethod
    def __prepare__(cls, name, bases, **kwargs):
        # list constant names here so that they appear in the help text of
        # the message class under "Data and other attributes defined here:"
        # as well as populate each message instance
        return {
        }


class DenseInputWithFeatures(metaclass=Metaclass_DenseInputWithFeatures):
    """Message class 'DenseInputWithFeatures'."""

    __slots__ = [
        '_header',
        '_frame_id',
        '_pose',
        '_image',
        '_min_depth',
        '_max_depth',
        '_features',
    ]

    _fields_and_field_types = {
        'header': 'std_msgs/Header',
        'frame_id': 'uint32',
        'pose': 'geometry_msgs/Pose',
        'image': 'sensor_msgs/Image',
        'min_depth': 'float',
        'max_depth': 'float',
        'features': 'sequence<svo_msgs/Feature>',
    }

    SLOT_TYPES = (
        rosidl_parser.definition.NamespacedType(['std_msgs', 'msg'], 'Header'),  # noqa: E501
        rosidl_parser.definition.BasicType('uint32'),  # noqa: E501
        rosidl_parser.definition.NamespacedType(['geometry_msgs', 'msg'], 'Pose'),  # noqa: E501
        rosidl_parser.definition.NamespacedType(['sensor_msgs', 'msg'], 'Image'),  # noqa: E501
        rosidl_parser.definition.BasicType('float'),  # noqa: E501
        rosidl_parser.definition.BasicType('float'),  # noqa: E501
        rosidl_parser.definition.UnboundedSequence(rosidl_parser.definition.NamespacedType(['svo_msgs', 'msg'], 'Feature')),  # noqa: E501
    )

    def __init__(self, **kwargs):
        assert all('_' + key in self.__slots__ for key in kwargs.keys()), \
            'Invalid arguments passed to constructor: %s' % \
            ', '.join(sorted(k for k in kwargs.keys() if '_' + k not in self.__slots__))
        from std_msgs.msg import Header
        self.header = kwargs.get('header', Header())
        self.frame_id = kwargs.get('frame_id', int())
        from geometry_msgs.msg import Pose
        self.pose = kwargs.get('pose', Pose())
        from sensor_msgs.msg import Image
        self.image = kwargs.get('image', Image())
        self.min_depth = kwargs.get('min_depth', float())
        self.max_depth = kwargs.get('max_depth', float())
        self.features = kwargs.get('features', [])

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
        if self.frame_id != other.frame_id:
            return False
        if self.pose != other.pose:
            return False
        if self.image != other.image:
            return False
        if self.min_depth != other.min_depth:
            return False
        if self.max_depth != other.max_depth:
            return False
        if self.features != other.features:
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
    def frame_id(self):
        """Message field 'frame_id'."""
        return self._frame_id

    @frame_id.setter
    def frame_id(self, value):
        if __debug__:
            assert \
                isinstance(value, int), \
                "The 'frame_id' field must be of type 'int'"
            assert value >= 0 and value < 4294967296, \
                "The 'frame_id' field must be an unsigned integer in [0, 4294967295]"
        self._frame_id = value

    @builtins.property
    def pose(self):
        """Message field 'pose'."""
        return self._pose

    @pose.setter
    def pose(self, value):
        if __debug__:
            from geometry_msgs.msg import Pose
            assert \
                isinstance(value, Pose), \
                "The 'pose' field must be a sub message of type 'Pose'"
        self._pose = value

    @builtins.property
    def image(self):
        """Message field 'image'."""
        return self._image

    @image.setter
    def image(self, value):
        if __debug__:
            from sensor_msgs.msg import Image
            assert \
                isinstance(value, Image), \
                "The 'image' field must be a sub message of type 'Image'"
        self._image = value

    @builtins.property
    def min_depth(self):
        """Message field 'min_depth'."""
        return self._min_depth

    @min_depth.setter
    def min_depth(self, value):
        if __debug__:
            assert \
                isinstance(value, float), \
                "The 'min_depth' field must be of type 'float'"
            assert not (value < -3.402823466e+38 or value > 3.402823466e+38) or math.isinf(value), \
                "The 'min_depth' field must be a float in [-3.402823466e+38, 3.402823466e+38]"
        self._min_depth = value

    @builtins.property
    def max_depth(self):
        """Message field 'max_depth'."""
        return self._max_depth

    @max_depth.setter
    def max_depth(self, value):
        if __debug__:
            assert \
                isinstance(value, float), \
                "The 'max_depth' field must be of type 'float'"
            assert not (value < -3.402823466e+38 or value > 3.402823466e+38) or math.isinf(value), \
                "The 'max_depth' field must be a float in [-3.402823466e+38, 3.402823466e+38]"
        self._max_depth = value

    @builtins.property
    def features(self):
        """Message field 'features'."""
        return self._features

    @features.setter
    def features(self, value):
        if __debug__:
            from svo_msgs.msg import Feature
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
                 all(isinstance(v, Feature) for v in value) and
                 True), \
                "The 'features' field must be a set or sequence and each value of type 'Feature'"
        self._features = value
