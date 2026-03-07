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
include_directories(../libraries/zf_components/tflm)
include_directories(../libraries/zf_components/tflm/third_party/flatbuffers/include)
include_directories(../libraries/zf_components/tflm/third_party/gemmlowp)
include_directories(../libraries/zf_components/tflm/third_party/gemmlowp/fixedpoint)
include_directories(../libraries/zf_components/tflm/third_party/ruy)
include_directories(../libraries/zf_components/tflm/third_party/ruy/ruy)
include_directories(../libraries/zf_components/tflm/tensorflow/lite/micro)
include_directories(../libraries/zf_components/tflm/tensorflow/lite/micro/kernels)
include_directories(../libraries/zf_components/tflm/tensorflow/lite/kernels/internal)
include_directories(../libraries/zf_components/tflm/tensorflow/lite/core/api)
include_directories(../libraries/zf_components/tflm/tensorflow/lite/core/c)
include_directories(../libraries/zf_components/tflm/tensorflow/lite/schema)

# TFLM sources
aux_source_directory(../libraries/zf_components/tflm/tensorflow/compiler/mlir/lite/core/api DIR_SRCS)
aux_source_directory(../libraries/zf_components/tflm/tensorflow/compiler/mlir/lite/schema DIR_SRCS)
aux_source_directory(../libraries/zf_components/tflm/tensorflow/lite/core/api DIR_SRCS)
aux_source_directory(../libraries/zf_components/tflm/tensorflow/lite/core/c DIR_SRCS)
aux_source_directory(../libraries/zf_components/tflm/tensorflow/lite/kernels DIR_SRCS)
aux_source_directory(../libraries/zf_components/tflm/tensorflow/lite/kernels/internal DIR_SRCS)
aux_source_directory(../libraries/zf_components/tflm/tensorflow/lite/micro DIR_SRCS)
aux_source_directory(../libraries/zf_components/tflm/tensorflow/lite/micro/cortex_m_generic DIR_SRCS)
aux_source_directory(../libraries/zf_components/tflm/tensorflow/lite/micro/arena_allocator DIR_SRCS)
aux_source_directory(../libraries/zf_components/tflm/tensorflow/lite/micro/memory_planner DIR_SRCS)
aux_source_directory(../libraries/zf_components/tflm/tensorflow/lite/micro/tflite_bridge DIR_SRCS)
aux_source_directory(../libraries/zf_components/tflm/tensorflow/lite/micro/kernels DIR_SRCS)

# Seekfree includes
include_directories(../libraries/zf_common)
include_directories(../libraries/zf_device)
include_directories(../libraries/zf_driver)
include_directories(../libraries/zf_components/seekfree_assistant)

# Seekfree sources
aux_source_directory(../libraries/zf_common DIR_SRCS)
aux_source_directory(../libraries/zf_device DIR_SRCS)
aux_source_directory(../libraries/zf_driver DIR_SRCS)
aux_source_directory(../libraries/zf_components/seekfree_assistant DIR_SRCS)

# Includes
include_directories(./src)

# Sources
aux_source_directory(./src DIR_SRCS)
aux_source_directory(./model DIR_SRCS)

# Create executable
add_executable(${PROJECT_NAME} ${DIR_SRCS})

# OpenCV linking
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
