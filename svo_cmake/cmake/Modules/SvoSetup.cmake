SET(CMAKE_BUILD_TYPE Release) # Release, RelWithDebInfo
#SET(CMAKE_BUILD_TYPE Debug) # Release, RelWithDebInfo
SET(CMAKE_VERBOSE_MAKEFILE OFF)

# user build settings
SET(USE_LOOP_CLOSING FALSE)
SET(USE_GLOBAL_MAP FALSE)

# Set definitions
IF(USE_LOOP_CLOSING)
  ADD_DEFINITIONS(-DSVO_LOOP_CLOSING)
ENDIF()

IF(USE_GLOBAL_MAP)
  ADD_DEFINITIONS(-DSVO_GLOBAL_MAP)
ENDIF()

ADD_DEFINITIONS(-DSVO_USE_ROS)
ADD_DEFINITIONS(-DSVO_USE_OPENGV)
ADD_DEFINITIONS(-DSVO_DEPTHFILTER_IN_REPROJECTOR)

#############################################################################
# Set build flags, set ARM_ARCHITECTURE environment variable on Odroid
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -Wall -Werror -D_LINUX -D_REENTRANT -march=native -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unknown-pragmas -Wno-unused-but-set-parameter -Wno-int-in-bool-context -Wno-maybe-uninitialized -Wno-unused-function -Wno-deprecated-declarations -Wno-class-memaccess")

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
    # ARM64 - NEON is always available
    ADD_DEFINITIONS(-DHAVE_FAST_NEON)
    MESSAGE(STATUS "Building for ARM64 (aarch64) architecture with NEON support")
  ELSE()
    # ARM32 - need to explicitly enable NEON
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mfpu=neon -march=armv7-a")
    ADD_DEFINITIONS(-DHAVE_FAST_NEON)
    MESSAGE(STATUS "Building for ARM32 architecture with NEON support")
  ENDIF()
ELSE()
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mmmx -msse -msse2 -msse3 -mssse3 -mno-avx")
  MESSAGE(STATUS "Building for x86 architecture with SSE support")
ENDIF()
if(NOT CMAKE_CXX_STANDARD)
  SET(CMAKE_CXX_STANDARD 17)
endif()
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -fsee -fomit-frame-pointer -fno-signed-zeros -fno-math-errno -funroll-loops -ffast-math -fno-finite-math-only")