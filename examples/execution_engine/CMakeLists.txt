set(LLVM_OPTIONAL_SOURCES
  main.cpp
)

add_llvm_executable(aiu-ee
  main.cpp
)
llvm_update_compile_flags(aiu-ee)

get_property(dialect_libs GLOBAL PROPERTY MLIR_DIALECT_LIBS)
target_link_libraries(aiu-ee
  PRIVATE
  AIUniteClient
  MLIRCAPIIR
  MLIRFuncDialect
  MLIRTosaDialect
)

