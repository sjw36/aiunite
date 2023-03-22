set(LLVM_OPTIONAL_SOURCES
  main.cpp
)

add_llvm_executable(aiu-kgenerator
  main.cpp
)
llvm_update_compile_flags(aiu-kgenerator)

get_property(dialect_libs GLOBAL PROPERTY MLIR_DIALECT_LIBS)
target_link_libraries(aiu-kgenerator
  PRIVATE
  AIUniteServer
  MLIRCAPIIR
  MLIRFuncDialect
  MLIRTosaDialect
)

