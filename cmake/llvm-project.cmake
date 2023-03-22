message(STATUS "Adding LLVM git-submodule src dependency")

# Passed to lit.site.cfg.py.so that the out of tree Standalone dialect test
# can find MLIR's CMake configuration
set(MLIR_CMAKE_CONFIG_DIR
   "${CMAKE_BINARY_DIR}/lib${LLVM_LIBDIR_SUFFIX}/cmake/mlir")

# MLIR settings
set(MLIR_TABLEGEN_EXE mlir-tblgen)

# LLVM settings
set(LLVM_ENABLE_PROJECTS "mlir;lld" CACHE STRING "List of default llvm targets")
set(LLVM_BUILD_EXAMPLES ON CACHE BOOL "")
set(LLVM_INSTALL_UTILS ON CACHE BOOL "")
set(LLVM_ENABLE_TERMINFO OFF CACHE BOOL "")
set(LLVM_ENABLE_ASSERTIONS ON CACHE BOOL "")
set(LLVM_PROJ_SRC "${CMAKE_SOURCE_DIR}/external/llvm-project")

# Cmake module paths
list(APPEND CMAKE_MODULE_PATH
  "${CMAKE_CURRENT_BINARY_DIR}/lib/cmake/mlir"
)
list(APPEND CMAKE_MODULE_PATH
  "${CMAKE_CURRENT_BINARY_DIR}/external/llvm-project/llvm/lib/cmake/llvm/"
)

# Include dirs for MLIR and LLVM
list(APPEND MLIR_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/external/llvm-project/mlir/include
  ${CMAKE_CURRENT_BINARY_DIR}/external/llvm-project/llvm/tools/mlir/include
)
list(APPEND LLVM_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/external/llvm-project/llvm/include
  ${CMAKE_CURRENT_BINARY_DIR}/external/llvm-project/llvm/include
)

# Linker flags
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath -Wl,${CMAKE_CURRENT_BINARY_DIR}/external/llvm-project/llvm/lib")

add_subdirectory("${LLVM_PROJ_SRC}/llvm" "external/llvm-project/llvm" EXCLUDE_FROM_ALL)
