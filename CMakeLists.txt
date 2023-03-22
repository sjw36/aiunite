cmake_minimum_required(VERSION 3.15.1)

# Allow VERSION for projects, as expected in CMake 3.0+
cmake_policy(SET CMP0048 NEW)
project(AIUnite VERSION 1.1.0 LANGUAGES CXX C)

# New in CMake 3.20. https://cmake.org/cmake/help/latest/policy/CMP0116.html
if(POLICY CMP0116)
  cmake_policy(SET CMP0116 OLD)
endif()

# Adapted from https://blog.kitware.com/cmake-and-the-default-build-type/
set(default_build_type "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
      STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Set policy CMP0057 to support IN_LIST operators
cmake_policy(SET CMP0057 NEW)

set(LLVM_ENABLE_ZLIB "OFF" CACHE STRING "")
set(LLVM_ENABLE_TERMINFO OFF CACHE BOOL "")

# LLVM settings that have an effect on the MLIR dialect
set(LLVM_TARGETS_TO_BUILD "" CACHE STRING "")

# Pointers to: 1) external LLVM bins/libs, and 2) AIUnite bins/libs
set(LLVM_MAIN_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/llvm-project/llvm" CACHE PATH "Path to LLVM sources")
set(LLVM_EXTERNAL_BIN_DIR "${CMAKE_CURRENT_BINARY_DIR}/external/llvm-project/llvm/bin" CACHE PATH "")
set(LLVM_EXTERNAL_LIB_DIR "${CMAKE_CURRENT_BINARY_DIR}/external/llvm-project/llvm/lib" CACHE PATH "")
set(AIUNITE_BIN_DIR "${CMAKE_CURRENT_BINARY_DIR}/bin" CACHE PATH "")
set(AIUNITE_LIB_DIR "${CMAKE_CURRENT_BINARY_DIR}/lib" CACHE PATH "")
message(STATUS "LLVM_EXTERNAL_BIN_DIR: ${LLVM_EXTERNAL_BIN_DIR}")
message(STATUS "AIUNITE_BIN_DIR: ${AIUNITE_BIN_DIR}")

# Update the build-tree RPATH
set(CMAKE_BUILD_RPATH "${AIUNITE_LIB_DIR};${LLVM_EXTERNAL_LIB_DIR}")
message(STATUS "CMAKE_BUILD_RPATH: ${CMAKE_BUILD_RPATH}")

# Library type and linkage settings
if( NOT DEFINED BUILD_FAT_LIBAIUNITE )
  set(BUILD_FAT_LIBAIUNITE OFF CACHE BOOL "Build fat libaiunite to link into AIUnite driver")
endif()

if( BUILD_FAT_LIBAIUNITE )
  set(BUILD_SHARED_LIBS OFF CACHE BOOL "")
  set(LLVM_BUILD_LLVM_DYLIB OFF CACHE BOOL "")
  set(MLIR_INCLUDE_INTEGRATION_TESTS OFF CACHE BOOL "")
  # Note, this is a hack to ignore Pytorch added conda path
  list(APPEND CMAKE_IGNORE_PATH /opt/conda)
else()
  set(BUILD_SHARED_LIBS ON CACHE BOOL "")
  set(LLVM_BUILD_LLVM_DYLIB ON CACHE BOOL "")
  set(LLVM_BUILD_EXAMPLES ON CACHE BOOL "")
  set(MLIR_INCLUDE_INTEGRATION_TESTS OFF CACHE BOOL "")
endif()

# Set up the build for the LLVM/MLIR git-submodule
include(cmake/llvm-project.cmake)

message(STATUS "LLVM_INCLUDE_DIRS: ${LLVM_INCLUDE_DIRS}")
message(STATUS "MLIR_INCLUDE_DIRS: ${MLIR_INCLUDE_DIRS}")
message(STATUS "LLVM_BUILD_LIBRARY_DIR: ${LLVM_BUILD_LIBRARY_DIR}")
message(STATUS "LLVM_DEFINITIONS: ${LLVM_DEFINITIONS}")

set(LLVM_RUNTIME_OUTPUT_INTDIR ${CMAKE_BINARY_DIR}/bin)
set(LLVM_LIBRARY_OUTPUT_INTDIR ${CMAKE_BINARY_DIR}/lib)
set(MLIR_BINARY_DIR ${CMAKE_BINARY_DIR})
set(MLIR_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

list(APPEND CMAKE_MODULE_PATH "${MLIR_CMAKE_DIR}")
list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(TableGen)
include(AddLLVM)
include(AddMLIR)
include(HandleLLVMOptions)

# Forbid implicit function declaration: this may lead to subtle bugs and we
# don't have a reason to support this.
check_c_compiler_flag("-Werror=implicit-function-declaration" C_SUPPORTS_WERROR_IMPLICIT_FUNCTION_DECLARATION)
append_if(C_SUPPORTS_WERROR_IMPLICIT_FUNCTION_DECLARATION "-Werror=implicit-function-declaration" CMAKE_C_FLAGS)

# Refer to https://reviews.llvm.org/D122088
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(${LLVM_INCLUDE_DIRS})
include_directories(${MLIR_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR}/include)
#include_directories(${PROJECT_BINARY_DIR}/external/llvm-project/llvm/tools/mlir/include)
link_directories(${LLVM_BUILD_LIBRARY_DIR})
add_definitions(${LLVM_DEFINITIONS})

# Set up the build for the AIUnite dialects
add_subdirectory(src)
add_subdirectory(examples)
