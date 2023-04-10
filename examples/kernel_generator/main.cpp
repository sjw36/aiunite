
#include <aiunite/provider.h>

#include <mlir/CAPI/IR.h>

#include <mlir/Dialect/Func/IR/FuncOps.h>
#include <mlir/Dialect/Tosa/IR/TosaOps.h>
#include <mlir/Parser/Parser.h>
#include <mlir/Support/FileUtilities.h>

#include <llvm/Support/SourceMgr.h>

#include <string>
#include <unordered_map>

AIUResponseCode service_cb(AIURequest request, AIUSolution solution) {

  static std::unordered_map<std::string, mlir::ModuleOp> md5Map;
  // mod.dump();

  switch (AIUGetRequestCode(request)) {
  case AIU_REQUEST_GET: {
    // lookup or compile
    std::string md5 = AIUGetMD5(request);
    auto lu = md5Map.find(md5);
    mlir::ModuleOp mod;

    if (!md5.empty()) {
      if (lu != md5Map.end()) {
        mod = lu->second;
      } else {
        mod = unwrap(AIUGetModule(request));
        md5Map[md5] = mod;
      }
    } else {
      mod = unwrap(AIUGetModule(request));
    }
    AIUSendModule(solution, wrap(mod));
    break;
  }
  case AIU_REQUEST_TUNE:
    break;
  case AIU_REQUEST_PARTITION:
    break;
  }

  return AIU_RESPONSE_SUCCESS;
}

int main(int argc, char **argv) {

  AIUCreateService(8000, service_cb);

  return 0;
}
