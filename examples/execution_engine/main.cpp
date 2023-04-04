
#include <aiunite/client.h>

#include <mlir/CAPI/IR.h>

#include <mlir/Dialect/Func/IR/FuncOps.h>
#include <mlir/Dialect/Tosa/IR/TosaOps.h>
#include <mlir/Parser/Parser.h>
#include <mlir/Support/FileUtilities.h>

#include <llvm/Support/SourceMgr.h>

#include <string>

int main(int argc, char **argv) {

  AIUContext ctx;
  AIUCreateContext(&ctx);

  AIUModel model;

  if (argc > 1) {
    std::string errorMessage;

    mlir::DialectRegistry registry;
    registry.insert<mlir::func::FuncDialect, mlir::tosa::TosaDialect>();
    mlir::MLIRContext context(registry);

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
        [&](mlir::func::FuncOp f) { AIUCloneModel(ctx, wrap(&*f), &model); });
  } else {
    int64_t t0dims[4] = {1, 32, 32, 256};
    int64_t t1dims[4] = {128, 3, 3, 256};
    AIUType t0type, t1type;
    AIU_CHECK_SUCCESS(AIUGetTensorType(ctx, 4, t0dims, AIU_F32, &t0type));
    AIU_CHECK_SUCCESS(AIUGetTensorType(ctx, 4, t1dims, AIU_F32, &t1type));
    // func foo (%arg0, %arg1, %arg2) -> tensor<xf32> {
    //   %cst = tosa.const <val> : type
    //   %0 = tosa.conv2d(%arg1, %arg2, %cst) attrs {...}
    //   %1 = tosa.add(%arg3, %0)
    //   return %1
    // }
    AIU_CHECK_SUCCESS(AIUCreateModel(ctx, "foo", t0type, &model));
    AIUValue param0;
    AIU_CHECK_SUCCESS(AIUAddParameter(model, t0type, &param0));
    AIUValue param1;
    AIU_CHECK_SUCCESS(AIUAddParameter(model, t1type, &param1));
    AIUValue param2;
    AIU_CHECK_SUCCESS(AIUAddParameter(model, t0type, &param2));
    AIUValue params[] = {param0, param1};
    AIUValue addResult;
    //AIU_CHECK_SUCCESS(AIUAddConstantSplat(model, AIU_, 2, params, t0type, &addResult));
    AIU_CHECK_SUCCESS(AIUAddOperation(model, AIU_ADD, 2, params, t0type, &addResult));
    AIU_CHECK_SUCCESS(AIUSetReturn(model, addResult));
  }

  AIURequest request;
  AIU_CHECK_SUCCESS(AIUSendModel(model, AIU_REQUEST_GET, &request));

  // do something else...

  AIUSolution solution;
  AIU_CHECK_SUCCESS(AIURecvSolution(request, &solution));

  return 0;
}
