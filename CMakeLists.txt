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

option(AIUNITE_ENABLE_EXCEPTIONS "Disable exception handling in boost" ON)
option(AIUNITE_ENABLE_RTTI "Disable exception handling in boost" ON)

# Pointers to: 1) external LLVM bins/libs, and 2) AIUnite bins/libs
set(AIUNITE_BIN_DIR "${CMAKE_CURRENT_BINARY_DIR}/bin" CACHE PATH "")
set(AIUNITE_LIB_DIR "${CMAKE_CURRENT_BINARY_DIR}/lib" CACHE PATH "")
message(STATUS "AIUNITE_BIN_DIR: ${AIUNITE_BIN_DIR}")
message(STATUS "AIUNITE_LIB_DIR: ${AIUNITE_LIB_DIR}")

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

# Update the build-tree RPATH
set(CMAKE_BUILD_RPATH "${AIUNITE_LIB_DIR};${LLVM_EXTERNAL_LIB_DIR}")
message(STATUS "CMAKE_BUILD_RPATH: ${CMAKE_BUILD_RPATH}")

option(AIU_ENABLE_LOGGING "Enable logging" ON)
if(AIU_ENABLE_LOGGING)
  add_compile_definitions(AIU_LOGGING_ENABLED)
endif()

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/internal)

# Set up the build for the AIUnite dialects
add_subdirectory(src)
add_subdirectory(examples)
