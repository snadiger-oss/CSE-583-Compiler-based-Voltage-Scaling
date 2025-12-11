cmake_minimum_required(VERSION 3.20)
project(VoltageScalingProject LANGUAGES C CXX)

find_package(LLVM REQUIRED CONFIG)

list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(AddLLVM)
include_directories(${LLVM_INCLUDE_DIRS})

add_subdirectory(custom_passes)
