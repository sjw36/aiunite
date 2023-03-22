
#include <aiunite/client.h>

#include <mlir/CAPI/IR.h>

#include <mlir/Dialect/Func/IR/FuncOps.h>
#include <mlir/Dialect/Tosa/IR/TosaOps.h>
#include <mlir/Parser/Parser.h>
#include <mlir/Support/FileUtilities.h>

#include <llvm/Support/SourceMgr.h>
#if 0
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/ErrorHandling.h>
#include <llvm/Support/InitLLVM.h>
#include <llvm/Support/ToolOutputFile.h>
#include <llvm/Support/raw_ostream.h>
#endif

#include <string>

int main(int argc, char **argv) {

  mlir::DialectRegistry registry;
  registry.insert<mlir::func::FuncDialect, mlir::tosa::TosaDialect>();
  mlir::MLIRContext context(registry);

  mlir::ModuleOp module;

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
    module = moduleRef.release();
  }
  
  AIUContext ctx;
  AIUCreateContext(&ctx);

  AIUFunc func;
  if (module) {
    module.walk([&](mlir::func::FuncOp f) {
        AIUCloneFunc(ctx, wrap(&*f), &func);
      });
  } else {
    AIUCreateFunc(ctx, "foo", &func);
  }

  AIURequest request;
  AIUSubmitFunc(ctx, func, 0, &request);

  return 0;
}
