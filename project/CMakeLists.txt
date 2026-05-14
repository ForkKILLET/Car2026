# ==============================
# Project
# ==============================
cmake_minimum_required(VERSION 3.10)
project(final-phase VERSION 0.1.0 LANGUAGES C CXX)
include(FetchContent)

# ==============================
# Load user config
# ==============================
include(config.cmake)

# ==============================
# Target selection (default: board)
# ==============================
if(NOT DEFINED TARGET)
  set(TARGET "board")
endif()
message(STATUS "CMake TARGET = ${TARGET}")

# ==============================
# Compiler settings
# ==============================
if(${TARGET} STREQUAL "board")
  if(NOT DEFINED CROSS_TOOLCHAIN_PREFIX)
    message(FATAL_ERROR "CROSS_TOOLCHAIN_PREFIX must be defined for board target")
  endif()

  # Cross compilation
  set(CMAKE_SYSTEM_NAME Linux)
  set(CMAKE_SYSTEM_PROCESSOR loongson)
  set(CMAKE_CXX_COMPILER ${CROSS_TOOLCHAIN_PREFIX}/bin/loongarch64-linux-gnu-g++)
  set(CMAKE_C_COMPILER ${CROSS_TOOLCHAIN_PREFIX}/bin/loongarch64-linux-gnu-gcc)

  set(CMAKE_FIND_ROOT_PATH ${CROSS_TOOLCHAIN_PREFIX})
  set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
  set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
  set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

  set(CMAKE_LINK_DEPENDS_NO_SHARED ON) # ld in the cross compilation toolchain does not support shared link

  # Architecture flags
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=loongarch64 -mtune=loongarch64 -fPIC -ffunction-sections -fdata-sections -Wall -pthread")
  set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -O3 -march=loongarch64 -mtune=loongarch64 -fPIC -ffunction-sections -fdata-sections -Wall")

else()
  # Native compiler
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall -std=c++17 -pthread")
  set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -O2 -Wall")
endif()

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ==============================
# Dep: OpenCV
# ==============================
if (${TARGET} STREQUAL "board")
  set(CMAKE_PREFIX_PATH ${OPENCV_PREFIX})
endif()
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# ==============================
# Include directories
# ==============================

# NCNN
include_directories(./lib/zf/zf_components/ncnn/include/ncnn/)
link_directories(./lib/zf/zf_components/ncnn/lib)

# TFLM
include_directories(
  ./lib/zf/zf_components/tflm
  ./lib/zf/zf_components/tflm/third_party/flatbuffers/include
  ./lib/zf/zf_components/tflm/third_party/gemmlowp
  ./lib/zf/zf_components/tflm/third_party/gemmlowp/fixedpoint
  ./lib/zf/zf_components/tflm/third_party/ruy
  ./lib/zf/zf_components/tflm/third_party/ruy/ruy
  ./lib/zf/zf_components/tflm/tensorflow/lite/micro
  ./lib/zf/zf_components/tflm/tensorflow/lite/micro/kernels
  ./lib/zf/zf_components/tflm/tensorflow/lite/kernels/internal
  ./lib/zf/zf_components/tflm/tensorflow/lite/core/api
  ./lib/zf/zf_components/tflm/tensorflow/lite/core/c
  ./lib/zf/zf_components/tflm/tensorflow/lite/schema
)

# Seekfree
include_directories(
  ./lib/zf/zf_common
  ./lib/zf/zf_device
  ./lib/zf/zf_driver
  ./lib/zf/zf_components/seekfree_assistant
)

# INIH
include_directories(./lib/inih)

# Project
include_directories(
  ./src
  ./src/debug
  ./model
)

# ==============================
# Collect source files
# ==============================

# TFLM
aux_source_directory(./lib/zf/zf_components/tflm/tensorflow/compiler/mlir/lite/core/api, SRC_TFLM)
aux_source_directory(./lib/zf/zf_components/tflm/tensorflow/compiler/mlir/lite/schema, SRC_TFLM)
aux_source_directory(./lib/zf/zf_components/tflm/tensorflow/lite/core/api, SRC_TFLM)
aux_source_directory(./lib/zf/zf_components/tflm/tensorflow/lite/core/c, SRC_TFLM)
aux_source_directory(./lib/zf/zf_components/tflm/tensorflow/lite/kernels, SRC_TFLM)
aux_source_directory(./lib/zf/zf_components/tflm/tensorflow/lite/kernels/internal, SRC_TFLM)
aux_source_directory(./lib/zf/zf_components/tflm/tensorflow/lite/micro, SRC_TFLM)
aux_source_directory(./lib/zf/zf_components/tflm/tensorflow/lite/micro/cortex_m_generic, SRC_TFLM)
aux_source_directory(./lib/zf/zf_components/tflm/tensorflow/lite/micro/arena_allocator, SRC_TFLM)
aux_source_directory(./lib/zf/zf_components/tflm/tensorflow/lite/micro/memory_planner, SRC_TFLM)
aux_source_directory(./lib/zf/zf_components/tflm/tensorflow/lite/micro/tflite_bridge, SRC_TFLM)
aux_source_directory(./lib/zf/zf_components/tflm/tensorflow/lite/micro/kernels, SRC_TFLM)

# Seekfree
aux_source_directory(./lib/zf/zf_common SRC_SEEKFREE)
aux_source_directory(./lib/zf/zf_device SRC_SEEKFREE)
aux_source_directory(./lib/zf/zf_driver SRC_SEEKFREE)

# Project
aux_source_directory(./model SRC_COMMON)
aux_source_directory(./src SRC_COMMON)

# ==============================
# Targets
# ==============================
if(${TARGET} STREQUAL "board")
  set(SRC_BOARD ${SRC_COMMON} ${SRC_TFLM} ${SRC_SEEKFREE})
  add_executable(main ./src/board_targets/main.cpp ${SRC_BOARD})
  target_link_libraries(main
    m
    pthread
    ${OpenCV_LIBS}
  )
elseif(${TARGET} STREQUAL "dev")
  set(SRC_DEV ${SRC_COMMON})
  add_executable(vision-debug ./src/dev_targets/vision_debug.cpp ${SRC_DEV})
  target_compile_definitions(vision-debug PRIVATE
    IMAGE_DEBUG
  )
  target_link_libraries(vision-debug
    m
    pthread
  )
endif()

# ==============================
# Dep: jsoncpp
# ==============================
FetchContent_Declare(
  jsoncpp
  GIT_REPOSITORY https://github.com/open-source-parsers/jsoncpp.git
  GIT_TAG 1.9.5
)

set(JSONCPP_WITH_TESTS OFF CACHE BOOL "" FORCE)
set(JSONCPP_WITH_POST_BUILD_UNITTEST OFF CACHE BOOL "" FORCE)
set(JSONCPP_WITH_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(jsoncpp)

if(${TARGET} STREQUAL "dev")
  target_link_libraries(vision-debug jsoncpp_lib)
endif()

# ==============================
# Dep: asio
# ==============================

FetchContent_Declare(
  asio
  GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
  GIT_TAG asio-1-36-0
)

FetchContent_MakeAvailable(asio)

if(${TARGET} STREQUAL "dev")
  target_compile_definitions(vision-debug PRIVATE
    ASIO_STANDALONE
  )
  target_include_directories(vision-debug PRIVATE
    ${asio_SOURCE_DIR}/asio/include
  )
endif()
