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
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -pthread -Wall -Wno-delete-non-virtual-dtor -Wno-placement-new")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=loongarch64 -mtune=loongarch64 -fPIC -ffunction-sections -fdata-sections")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=loongarch64 -mtune=loongarch64 -fPIC -ffunction-sections -fdata-sections")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections")

# Add OpenCV prefix
set(CMAKE_PREFIX_PATH ${OPENCV_PREFIX})
find_package(OpenCV REQUIRED)

# OpenCV includes
include_directories(${OpenCV_INCLUDE_DIRS})

# NCNN includes
include_directories(../libraries/zf_components/ncnn/include/ncnn/)

# NCNN linking
link_directories(../libraries/zf_components/ncnn/lib)

# TFLM includes
include_directories(
  ../libraries/zf_components/tflm
  ../libraries/zf_components/tflm/third_party/flatbuffers/include
  ../libraries/zf_components/tflm/third_party/gemmlowp
  ../libraries/zf_components/tflm/third_party/gemmlowp/fixedpoint
  ../libraries/zf_components/tflm/third_party/ruy
  ../libraries/zf_components/tflm/third_party/ruy/ruy
  ../libraries/zf_components/tflm/tensorflow/lite/micro
  ../libraries/zf_components/tflm/tensorflow/lite/micro/kernels
  ../libraries/zf_components/tflm/tensorflow/lite/kernels/internal
  ../libraries/zf_components/tflm/tensorflow/lite/core/api
  ../libraries/zf_components/tflm/tensorflow/lite/core/c
  ../libraries/zf_components/tflm/tensorflow/lite/schema
)

# TFLM sources
aux_source_directory(../libraries/zf_components/tflm/tensorflow/compiler/mlir/lite/core/api SOURCES)
aux_source_directory(../libraries/zf_components/tflm/tensorflow/compiler/mlir/lite/schema SOURCES)
aux_source_directory(../libraries/zf_components/tflm/tensorflow/lite/core/api SOURCES)
aux_source_directory(../libraries/zf_components/tflm/tensorflow/lite/core/c SOURCES)
aux_source_directory(../libraries/zf_components/tflm/tensorflow/lite/kernels SOURCES)
aux_source_directory(../libraries/zf_components/tflm/tensorflow/lite/kernels/internal SOURCES)
aux_source_directory(../libraries/zf_components/tflm/tensorflow/lite/micro SOURCES)
aux_source_directory(../libraries/zf_components/tflm/tensorflow/lite/micro/cortex_m_generic SOURCES)
aux_source_directory(../libraries/zf_components/tflm/tensorflow/lite/micro/arena_allocator SOURCES)
aux_source_directory(../libraries/zf_components/tflm/tensorflow/lite/micro/memory_planner SOURCES)
aux_source_directory(../libraries/zf_components/tflm/tensorflow/lite/micro/tflite_bridge SOURCES)
aux_source_directory(../libraries/zf_components/tflm/tensorflow/lite/micro/kernels SOURCES)

# Seekfree includes
include_directories(../libraries/zf_common)
include_directories(../libraries/zf_device)
include_directories(../libraries/zf_driver)
include_directories(../libraries/zf_components/seekfree_assistant)

# Seekfree sources
aux_source_directory(../libraries/zf_common SOURCES)
aux_source_directory(../libraries/zf_device SOURCES)
aux_source_directory(../libraries/zf_driver SOURCES)
aux_source_directory(../libraries/zf_components/seekfree_assistant SOURCES)

# Includes
include_directories(./model)
include_directories(./src)

# Sources
aux_source_directory(./model SOURCES)
aux_source_directory(./src SOURCES)
aux_source_directory(./src/tests SOURCES)
aux_source_directory(./src/camera SOURCES)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# OpenCV linking
target_link_libraries(
  ${PROJECT_NAME}
  # m
  # pthread
  # ${OpenCV_LIBS}
  # tflm
)
