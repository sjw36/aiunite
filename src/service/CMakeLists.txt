set(LLVM_OPTIONAL_SOURCES
  service.cpp
)

llvm_add_library(AIUniteServer
PARTIAL_SOURCES_INTENDED

  service.cpp

  LINK_LIBS
  AIUniteCommon
  MLIRCAPIIR
  MLIRFuncDialect
  MLIRTosaDialect
  ssl
  crypto
  
  LINK_COMPONENTS
)

target_compile_options(AIUniteServer PUBLIC -fexceptions)

