set(LLVM_OPTIONAL_SOURCES
  build.cpp
  device_request.cpp
  devices.cpp
  solution.cpp
)

llvm_add_library(AIUniteClient
PARTIAL_SOURCES_INTENDED

  build.cpp
  device_request.cpp
  devices.cpp
  solution.cpp

  LINK_LIBS
  AIUniteCommon
  MLIRCAPIIR
  MLIRFuncDialect
  MLIRTosaDialect
  MLIRXModel

  boost_date_time
  boost_exception
  boost_filesystem
  boost_log
  boost_log_setup
  boost_system
  boost_thread
  boost_chrono
  boost_atomic
  
  LINK_COMPONENTS
)

target_compile_options(AIUniteClient PUBLIC -fexceptions -frtti)

