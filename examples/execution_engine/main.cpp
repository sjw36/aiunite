
#include <aiunite/client.h>

#include <mlir/CAPI/IR.h>

#include <mlir/Dialect/Func/IR/FuncOps.h>
#include <mlir/Dialect/Tosa/IR/TosaOps.h>
#include <mlir/Parser/Parser.h>
#include <mlir/Support/FileUtilities.h>

#include <llvm/Support/SourceMgr.h>

#include <string>

int main(int argc, char **argv) {

  AIU_CHECK_SUCCESS(AIUInitialize());

  // must persist for cloned funcs...
  mlir::DialectRegistry registry;
  registry.insert<mlir::func::FuncDialect, mlir::tosa::TosaDialect>();
  mlir::MLIRContext context(registry);

  AIUModel model;

  if (argc > 1) {
    std::string errorMessage;

    auto file = mlir::openInputFile(argv[1], &errorMessage);
    if (!file) {
      llvm::errs() << errorMessage << "\n";
      exit(1);
    }

    // Parse the input file.
    llvm::SourceMgr sourceMgr;
    sourceMgr.AddNewSourceBuffer(std::move(file), llvm::SMLoc());
    mlir::OwningOpRef<mlir::ModuleOp> moduleRef =
        mlir::parseSourceFile<mlir::ModuleOp>(sourceMgr, &context);
    if (!moduleRef) {
      llvm::errs() << "Parse host harness " << argv[0] << " failed.\n";
      exit(1);
    }
    mlir::ModuleOp module = moduleRef.release();
    module.walk(
        [&](mlir::func::FuncOp f) { AIUCloneModel(wrap(&*f), &model); });
  } else {
    // func test (%arg0, %arg1, %arg2) -> tensor<xf32> {
    //   %cst = tosa.const <val> : type
    //   %0 = tosa.conv2d(%arg1, %arg2, %cst) attrs {...}
    //   %1 = tosa.add(%arg3, %0)
    //   return %1
    // }

    AIU_CHECK_SUCCESS(AIUCreateModel("test", &model));

    int64_t t0dims[4] = {128, 32, 32, 8};
    int64_t t1dims[4] = {128, 3, 3, 8};
    int64_t t2dims[1] = {128};
    int64_t t3dims[4] = {128, 30, 30, 128};
    AIUType t0type, t1type, t2type, t3type;
    AIU_CHECK_SUCCESS(AIUGetTensorType(model, 4, t0dims, AIU_F32, &t0type));
    AIU_CHECK_SUCCESS(AIUGetTensorType(model, 4, t1dims, AIU_F32, &t1type));
    AIU_CHECK_SUCCESS(AIUGetTensorType(model, 1, t2dims, AIU_F32, &t2type));
    AIU_CHECK_SUCCESS(AIUGetTensorType(model, 4, t3dims, AIU_F32, &t3type));
    AIUValue param0;
    AIU_CHECK_SUCCESS(AIUAddParameter(model, t0type, &param0));
    AIUValue param1;
    AIU_CHECK_SUCCESS(AIUAddParameter(model, t1type, &param1));
    AIUValue param2;
    AIU_CHECK_SUCCESS(AIUAddParameter(model, t3type, &param2));

    // make const
    AIUValue cstRes;
    float cstVal[] = {0.0};
    AIU_CHECK_SUCCESS(AIUAddConstantSplat(model, t2type, cstVal, &cstRes));

    // make attrs
    AIUAttr attrs[3];
    int64_t dilation[] = {1, 1};
    int64_t stride[] = {1, 1};
    int64_t pad[] = {0, 0, 0, 0};
    AIU_CHECK_SUCCESS(
        AIUMakeArrayAttr(model, AIU_DILATION_ATTR, 2, dilation, &attrs[0]));
    AIU_CHECK_SUCCESS(
        AIUMakeArrayAttr(model, AIU_STRIDE_ATTR, 2, stride, &attrs[1]));
    AIU_CHECK_SUCCESS(AIUMakeArrayAttr(model, AIU_PAD_ATTR, 4, pad, &attrs[2]));
    // make conv2d
    AIUValue convResult;
    AIUValue params[] = {param0, param1, cstRes};
    AIU_CHECK_SUCCESS(AIUAddOperationWithAttrs(model, AIU_CONV2D, 3, params, 3,
                                               attrs, t3type, &convResult));

    // make add
    AIUValue addResult;
    AIUValue addParams[] = {param2, convResult};
    AIU_CHECK_SUCCESS(
        AIUAddOperation(model, AIU_ADD, 2, addParams, t3type, &addResult));

    // make return
    AIU_CHECK_SUCCESS(AIUSetReturn(model, addResult));
  }

  AIURequest request;
  AIU_CHECK_SUCCESS(AIUSendModel(model, AIU_REQUEST_GET, &request));

  // do something else...

  AIUSolution solution;
  AIU_CHECK_SUCCESS(AIURecvSolution(request, &solution));

  return 0;
}
