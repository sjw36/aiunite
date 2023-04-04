set(LLVM_OPTIONAL_SOURCES
  build.cpp
  client.cpp
)

llvm_add_library(AIUniteClient
PARTIAL_SOURCES_INTENDED

  build.cpp
  client.cpp

  LINK_LIBS
  AIUniteCommon
  MLIRCAPIIR
  MLIRFuncDialect
  MLIRTosaDialect

  LINK_COMPONENTS
)

target_compile_options(AIUniteClient PUBLIC -fexceptions)

