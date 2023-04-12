set(LLVM_OPTIONAL_SOURCES
  protocol.cpp
  logger.cpp
)

llvm_add_library(AIUniteCommon
PARTIAL_SOURCES_INTENDED

  protocol.cpp
  logger.cpp
  
  LINK_LIBS
  pthread
  boost_date_time
  boost_exception
  boost_filesystem
  boost_log
  boost_log_setup
  boost_system
  boost_thread
  boost_chrono
  boost_atomic
  
)

target_compile_options(AIUniteCommon PUBLIC -fexceptions -frtti)

