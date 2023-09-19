set(LLVM_OPTIONAL_SOURCES
  build.cpp
  communication.cpp
  devices.cpp
  model.cpp
  solution.cpp
)

llvm_add_library(AIUniteClient
PARTIAL_SOURCES_INTENDED

  build.cpp
  communication.cpp
  devices.cpp
  model.cpp
  solution.cpp

  LINK_LIBS
  AIUniteCommon
  MLIRCAPIIR
  MLIRFuncDialect
  MLIRTosaDialect
  MLIRLinalgDialect
  MLIRMHAL

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

if(AIUNITE_ENABLE_EXCEPTIONS)
  target_compile_options(AIUniteClient PUBLIC -fexceptions)
else()
  add_definitions(-DBOOST_NO_EXCEPTIONS)
endif()
if(AIUNITE_ENABLE_RTTI)
  target_compile_options(AIUniteClient PUBLIC -frtti)
else()
  add_definitions(-DBOOST_NO_RTTI)
endif()

