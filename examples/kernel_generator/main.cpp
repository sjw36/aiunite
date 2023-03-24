
#include <aiunite/provider.h>

#include <mlir/CAPI/IR.h>

#include <mlir/Dialect/Func/IR/FuncOps.h>
#include <mlir/Dialect/Tosa/IR/TosaOps.h>
#include <mlir/Parser/Parser.h>
#include <mlir/Support/FileUtilities.h>

#include <llvm/Support/SourceMgr.h>

#include <string>

AIUResponseCode service_cb(AIURequest request, AIUSolution solution) {

  MlirModule c_mod = AIUGetModule(request);
  mlir::ModuleOp mod = unwrap(c_mod);
  // mod.dump();

  switch (AIUGetRequestCode(request)) {
  case AIU_REQUEST_GET:
    // lookup or compile
    AIUSetModule(solution, c_mod);
    break;
  case AIU_REQUEST_TUNE:
    break;
  case AIU_REQUEST_PARTITION:
    AIUSetModule(solution, c_mod);
    break;
  }

  return AIU_RESPONSE_SUCCESS;
}

int main(int argc, char **argv) {

  AIUCreateService(8000, service_cb);

  return 0;
}
