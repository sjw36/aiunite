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

target_compile_options(AIUniteService PUBLIC -fexceptions)

