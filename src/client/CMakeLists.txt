set(LLVM_OPTIONAL_SOURCES
  client.cpp
)

llvm_add_library(AIUniteClient
PARTIAL_SOURCES_INTENDED

  client.cpp

  LINK_LIBS
  MLIRCAPIIR
  MLIRFuncDialect
  MLIRTosaDialect

  LINK_COMPONENTS
)

target_compile_options(AIUniteClient PUBLIC -fexceptions)

