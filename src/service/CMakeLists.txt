set(LLVM_OPTIONAL_SOURCES
  service.cpp
)

llvm_add_library(AIUniteService
PARTIAL_SOURCES_INTENDED

  service.cpp

  LINK_LIBS
  AIUniteCommon
  MLIRCAPIIR
  MLIRFuncDialect
  MLIRTosaDialect
  ssl
  crypto

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

target_include_directories(AIUniteService
  PUBLIC
  $<BUILD_INTERFACE:${AIUNITE_PROJECT_DIR}/include>
  $<BUILD_INTERFACE:${AIUNITE_PROJECT_DIR}/internal>
)

if(AIUNITE_ENABLE_EXCEPTIONS)
  target_compile_options(AIUniteService PUBLIC -fexceptions)
else()
  add_definitions(-DBOOST_NO_EXCEPTIONS)
endif()
if(AIUNITE_ENABLE_RTTI)
  target_compile_options(AIUniteService PUBLIC -frtti)
else()
  add_definitions(-DBOOST_NO_RTTI)
endif()

