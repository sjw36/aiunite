
#include <aiunite/provider.h>

#include <mlir/CAPI/IR.h>

#include <mlir/Dialect/Func/IR/FuncOps.h>
#include <mlir/Dialect/Tosa/IR/TosaOps.h>
#include <mlir/Parser/Parser.h>
#include <mlir/Support/FileUtilities.h>

#include <llvm/Support/SourceMgr.h>

#include <string>

void service_cb(AIURequest request) {
  // MlirModule module = AIUGetModule(request);
  // mlir::ModuleOp mod = unwrap(module);
  // mod.dump();
  assert(0);
}

int main(int argc, char **argv) {

  AIUCreateService(8000, service_cb);

  return 0;
}
