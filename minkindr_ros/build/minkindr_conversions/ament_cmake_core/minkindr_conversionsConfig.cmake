# generated from ament/cmake/core/templates/nameConfig.cmake.in

# prevent multiple inclusion
if(_minkindr_conversions_CONFIG_INCLUDED)
  # ensure to keep the found flag the same
  if(NOT DEFINED minkindr_conversions_FOUND)
    # explicitly set it to FALSE, otherwise CMake will set it to TRUE
    set(minkindr_conversions_FOUND FALSE)
  elseif(NOT minkindr_conversions_FOUND)
    # use separate condition to avoid uninitialized variable warning
    set(minkindr_conversions_FOUND FALSE)
  endif()
  return()
endif()
set(_minkindr_conversions_CONFIG_INCLUDED TRUE)

# output package information
if(NOT minkindr_conversions_FIND_QUIETLY)
  message(STATUS "Found minkindr_conversions: 0.0.0 (${minkindr_conversions_DIR})")
endif()

# warn when using a deprecated package
if(NOT "" STREQUAL "")
  set(_msg "Package 'minkindr_conversions' is deprecated")
  # append custom deprecation text if available
  if(NOT "" STREQUAL "TRUE")
    set(_msg "${_msg} ()")
  endif()
  # optionally quiet the deprecation message
  if(NOT ${minkindr_conversions_DEPRECATED_QUIET})
    message(DEPRECATION "${_msg}")
  endif()
endif()

# flag package as ament-based to distinguish it after being find_package()-ed
set(minkindr_conversions_FOUND_AMENT_PACKAGE TRUE)

# include all config extra files
set(_extras "ament_cmake_export_targets-extras.cmake;ament_cmake_export_dependencies-extras.cmake")
foreach(_extra ${_extras})
  include("${minkindr_conversions_DIR}/${_extra}")
endforeach()
