cmake_minimum_required(VERSION 3.5)
project(fast_neon)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unknown-pragmas")

# Auto-detect architecture
IF(DEFINED ENV{ARM_ARCHITECTURE})
  SET(IS_ARM TRUE)
ELSEIF(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm.*)")
  SET(IS_ARM TRUE)
ELSE()
  SET(IS_ARM FALSE)
ENDIF()

IF(IS_ARM)
  # Check if ARM64 or ARM32
  IF(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
    # ARM64 - NEON is always available, but need to define the macro
    add_compile_definitions(__ARM_NEON__)
    MESSAGE(STATUS "Building for ARM64 (aarch64) architecture with NEON support")
  ELSE()
    # ARM32 - need to explicitly enable NEON
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mfpu=neon -march=armv7-a")
    MESSAGE(STATUS "Building for ARM32 architecture with NEON support")
  ENDIF()
ELSE()
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mmmx -msse -msse2 -msse3 -mssse3 -fomit-frame-pointer")
  MESSAGE(STATUS "Building for x86 architecture with SSE support")
ENDIF()

find_package(ament_cmake REQUIRED)

set(HEADERS
  include/fast/corner_9.h
  include/fast/corner_10.h
  include/fast/fast.h
  include/fast/faster_corner_utilities.h
  )

set(SOURCES
  src/fast_9.cpp
  src/fast_10.cpp
  src/fast_10_score.cpp
  src/nonmax_3x3.cpp
  )

IF(IS_ARM)
  LIST(APPEND SOURCES ${PROJECT_SOURCE_DIR}/src/faster_corner_9_neon.cpp)
ELSE()
  LIST(APPEND SOURCES ${PROJECT_SOURCE_DIR}/src/faster_corner_10_sse.cpp)
ENDIF()

add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

################################################################################
install(TARGETS ${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY include/
  DESTINATION include
)

ament_export_include_directories(include)
ament_export_libraries(${PROJECT_NAME})

ament_package()
