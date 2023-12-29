message(STATUS "Adding LLVM git-submodule src dependency")

option(AIUNITE_USE_LOCAL_LLVM "Use local LLVM in external/llvm-project" ON)

option(AIUNITE_LLVM_PROJECT_DIR "LLVM source dir" "${CMAKE_CURRENT_SOURCE_DIR}/external/llvm-project")
message(STATUS "AIUNITE_LLVM_PROJECT_DIR: ${AIUNITE_LLVM_PROJECT_DIR}")

option(AIUNITE_LLVM_BINARY_DIR "LLVM binary dir" "${CMAKE_CURRENT_BINARY_DIR}/external/llvm-project")
message(STATUS "AIUNITE_LLVM_BINARY_DIR: ${AIUNITE_LLVM_BINARY_DIR}")

set(LLVM_MAIN_SRC_DIR "${AIUNITE_LLVM_PROJECT_DIR}/llvm" CACHE PATH "Path to LLVM sources")
set(LLVM_EXTERNAL_BIN_DIR "${AIUNITE_LLVM_BINARY_DIR}/llvm/bin" CACHE PATH "")
set(LLVM_EXTERNAL_LIB_DIR "${AIUNITE_LLVM_BINARY_DIR}/llvm/lib" CACHE PATH "")


# Passed to lit.site.cfg.py.so that the out of tree Standalone dialect test
# can find MLIR's CMake configuration
set(MLIR_CMAKE_CONFIG_DIR
  "${AIUNITE_LLVM_BINARY_DIR}/lib${LLVM_LIBDIR_SUFFIX}/cmake/mlir")

# MLIR settings
set(MLIR_TABLEGEN_EXE mlir-tblgen)

# LLVM settings that have an effect on the MLIR dialect
set(LLVM_TARGETS_TO_BUILD "" CACHE STRING "")
set(LLVM_ENABLE_PROJECTS "mlir" CACHE STRING "List of default llvm targets")
set(LLVM_BUILD_EXAMPLES ON CACHE BOOL "")
set(LLVM_INSTALL_UTILS ON CACHE BOOL "")
set(LLVM_ENABLE_TERMINFO OFF CACHE BOOL "")
set(LLVM_ENABLE_ASSERTIONS ON CACHE BOOL "")
set(LLVM_ENABLE_ZLIB "OFF" CACHE STRING "")

# Cmake module paths
list(APPEND CMAKE_MODULE_PATH
  "${CMAKE_CURRENT_BINARY_DIR}/lib/cmake/mlir"
  )
list(APPEND CMAKE_MODULE_PATH
  "${AIUNITE_LLVM_BINARY_DIR}/llvm/lib/cmake/llvm/"
  )

# Include dirs for MLIR and LLVM
list(APPEND MLIR_INCLUDE_DIRS
  ${AIUNITE_LLVM_PROJECT_DIR}/mlir/include
  ${AIUNITE_LLVM_BINARY_DIR}/llvm/tools/mlir/include
  )
list(APPEND LLVM_INCLUDE_DIRS
  ${AIUNITE_LLVM_PROJECT_DIR}/llvm/include
  ${AIUNITE_LLVM_BINARY_DIR}/llvm/include
  )

message(STATUS "LLVM_INCLUDE_DIRS: ${LLVM_INCLUDE_DIRS}")
message(STATUS "MLIR_INCLUDE_DIRS: ${MLIR_INCLUDE_DIRS}")
message(STATUS "LLVM_BUILD_LIBRARY_DIR: ${LLVM_BUILD_LIBRARY_DIR}")
message(STATUS "LLVM_DEFINITIONS: ${LLVM_DEFINITIONS}")

set(LLVM_RUNTIME_OUTPUT_INTDIR ${CMAKE_BINARY_DIR}/bin)
set(LLVM_LIBRARY_OUTPUT_INTDIR ${CMAKE_BINARY_DIR}/lib)
set(MLIR_BINARY_DIR ${CMAKE_BINARY_DIR})
set(MLIR_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

if(AIUNITE_USE_LOCAL_LLVM)
  # Linker flags
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath -Wl,${CMAKE_CURRENT_BINARY_DIR}/external/llvm-project/llvm/lib")

  add_subdirectory("${AIUNITE_LLVM_PROJECT_DIR}/llvm" "external/llvm-project/llvm" EXCLUDE_FROM_ALL)

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

else()
endif()

include_directories(${LLVM_INCLUDE_DIRS})
include_directories(${MLIR_INCLUDE_DIRS})

link_directories(${LLVM_BUILD_LIBRARY_DIR})
add_definitions(${LLVM_DEFINITIONS})
