set(LLVM_OPTIONAL_SOURCES
  main.cpp
)

add_llvm_executable(aiu-kg
  main.cpp
)
llvm_update_compile_flags(aiu-kg)

get_property(dialect_libs GLOBAL PROPERTY MLIR_DIALECT_LIBS)
target_link_libraries(aiu-kg
  PRIVATE
  AIUniteServer
  MLIRCAPIIR
  MLIRFuncDialect
  MLIRTosaDialect
)

