/******************************************************************************/
/*  AIU:   OPEN Kernel Generator Interface                                   */
/*  AIKGI:  AI Kernel Generator Interface                                     */
/*  AIAPI:  AI Accelerator (Programming) Interface                            */
/******************************************************************************/


#include <aiunite/client.h>
#include <_aiu/client/solution.h>
#include <_aiu/support.h>
#include <_aiu/logger.h>

#include <mlir/CAPI/IR.h>

#include <mlir/Dialect/Func/IR/FuncOps.h>
#include <mlir/Dialect/Linalg/IR/Linalg.h>
#include <mlir/Dialect/MemRef/IR/MemRef.h>
#include <mlir/Dialect/Tosa/IR/TosaOps.h>
#include <mlir/Dialect/MHAL/IR/MHAL.h>
#include <mlir/Parser/Parser.h>

struct _AIUBinary {
  _AIUBinary(AIUDevice _dev, llvm::StringRef binary, llvm::ArrayRef<uint32_t> ldims)
    : device(_dev), bits(binary) {
    for (auto dim : ldims)
      launch_dims.push_back(dim);
  }
  const llvm::StringRef getBits() const { return bits; }
  const std::vector<size_t> getDims() const { return launch_dims; }
  const size_t *getDimsData() const { return launch_dims.data(); }
private:
  AIUDevice device;
  std::string type;
  llvm::StringRef bits;
  std::vector<size_t> launch_dims;
};

static mlir::DialectRegistry &getRegistry() {
  static mlir::DialectRegistry _reg;
  // call_once
  _reg.insert<mlir::func::FuncDialect>();
  _reg.insert<mlir::memref::MemRefDialect>();
  _reg.insert<mlir::mhal::MHALDialect>();
  _reg.insert<mlir::linalg::LinalgDialect>();
  return _reg;
}

_AIUSolution::_AIUSolution(AIUDevice dev, const std::string &body)
  : _device(dev), _context(new mlir::MLIRContext(getRegistry())) {
  _context->allowUnregisteredDialects();
  mlir::ParserConfig config(_context);
  auto moduleRef =
    mlir::parseSourceString<mlir::ModuleOp>(body.c_str(), config);
  if (moduleRef) {
    _d = moduleRef.release();
    _code = AIU_SOLUTION_SUCCESS;
    AIU_LOG_DBG(_d);
  } else {
    _code = AIU_SOLUTION_INVALID_MODEL;
  }
}

AIUBinary _AIUSolution::getBinary(const char *func_name_) {
  auto li = _binaries.find(func_name_);
  if (li != _binaries.end())
    return li->second;
  //lookup func by name
  if (auto func = _d.lookupSymbol<mlir::func::FuncOp>(func_name_)) {
    // get xmodel targets
    if (auto targets = func->getAttrOfType<mlir::ArrayAttr>("mhal.targets")) {
      assert(targets.getValue().size() == 1);
      for (auto targetAttr : targets.getValue()) {
        auto pkgAttr = targetAttr.cast<mlir::mhal::KernelPackageAttr>();
        // check arch and type
        // auto type = pkgAttr.getType();
        auto dims = pkgAttr.getLaunchDims();
        auto obj = pkgAttr.getObject();
        auto bin = new _AIUBinary(_device, obj.getBinary(), dims);
        _binaries[func_name_] = bin;
        AIU_LOG_DBG("AIUBinary: " << func_name_ << " " << obj.getBinary().size());
        return bin;
      }
    }
  }
  return nullptr;
}


/******************************************************************************/
/*  SOLUTION SPEC                                                             */
/******************************************************************************/
extern "C" AIUSolutionCode AIUGetSolutionCode(AIUSolution solution_) {
  AIU_LOG_FUNC(AIUGetSolutionCode);
  //AIU_CHECK_OBJECT(solution_);
  return solution_->getCode();
}

/* -- Generated kernels and call graph */
extern "C" MlirModule AIUGetModule(AIUSolution solution_) {
  AIU_LOG_FUNC(AIUGetModule);
  assert(solution_ != nullptr);
  return wrap(solution_->get());
}

/*   - Binary(s) */
extern "C" AIUResultCode AIUGetBinary(AIUSolution solution_,
                                      const char *func_name_,
                                      AIUBinary *result_) {
  AIU_LOG_FUNC(AIUGetBinary);
  AIU_CHECK_OBJECT(solution_);
  AIU_CHECK_RESULT(result_);

  *result_ = solution_->getBinary(func_name_);

  return *result_ == nullptr ? AIU_FAILURE : AIU_SUCCESS;
}

extern "C" AIUResultCode AIUGetObject(AIUBinary binary_, const char **buffer_) {
  AIU_LOG_FUNC(AIUGetObject);
  AIU_CHECK_OBJECT(binary_);
  AIU_CHECK_RESULT(buffer_);

  *buffer_ = binary_->getBits().data();
  return AIU_SUCCESS;
}

extern "C" AIUResultCode AIUGetLaunchDims(AIUBinary binary_, size_t *size_,
                                          const size_t **dims_) {
  AIU_LOG_FUNC(AIUGetLaunchDims);
  AIU_CHECK_OBJECT(binary_);
  AIU_CHECK_RESULT(size_);
  AIU_CHECK_RESULT(dims_);

  const auto &launch_dims = binary_->getDims();
  *size_ = launch_dims.size();
  *dims_ = binary_->getDimsData();

  return AIU_SUCCESS;
}

/* -- Tuning space */
