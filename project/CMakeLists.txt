# CMake settings
cmake_minimum_required(VERSION 3.10.0)

# Load and check config
include(config.cmake)

if(NOT DEFINED TOOLCHAIN_PREFIX)
  message(FATAL_ERROR "TOOLCHAIN_PREFIX is not set")
endif()

if(NOT DEFINED OPENCV_PREFIX)
  message(FATAL_ERROR "OPENCV_PREFIX is not set")
endif()

# Project
project(final-phase VERSION 0.1.0 LANGUAGES C CXX)

# Cross compilation settings
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR loongson)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}/bin/loongarch64-linux-gnu-g++)
set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}/bin/loongarch64-linux-gnu-gcc)

set(CMAKE_FIND_ROOT_PATH ${TOOLCHAIN_PREFIX})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(CMAKE_LINK_DEPENDS_NO_SHARED ON) # ld in the cross compilation toolchain does not support shared link

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# C/C++ Settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -pthread -Wall")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -Wall")

# Add OpenCV prefix
set(CMAKE_PREFIX_PATH ${OPENCV_PREFIX})
find_package(OpenCV REQUIRED)

# OpenCV includes
include_directories(${OpenCV_INCLUDE_DIRS})

# Other includes
include_directories(./src)
include_directories(../libraries/zf_common)
include_directories(../libraries/zf_device)
include_directories(../libraries/zf_driver)
include_directories(../libraries/zf_components)

# Source files
aux_source_directory(./src DIR_SRCS)
aux_source_directory(../libraries/zf_common DIR_SRCS)
aux_source_directory(../libraries/zf_device DIR_SRCS)
aux_source_directory(../libraries/zf_driver DIR_SRCS)
aux_source_directory(../libraries/zf_components DIR_SRCS)

# Create executable
add_executable(${PROJECT_NAME} ${DIR_SRCS})

# OpenCV linking
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
