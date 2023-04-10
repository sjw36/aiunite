/******************************************************************************/
/*  CPX:   AI Unite API                                                       */
/*  Common Pluggable eXchange                                                 */
/******************************************************************************/

#ifndef AIUNITE_INTERNAL_SOLUTION_H
#define AIUNITE_INTERNAL_SOLUTION_H

#include <mlir/Dialect/Func/IR/FuncOps.h>
#include <mlir/Dialect/Tosa/IR/TosaOps.h>

struct _AIUSolution {

  mlir::DialectRegistry &getRegistry() {
    static mlir::DialectRegistry _reg;
    // call_once
    _reg.insert<mlir::func::FuncDialect>();
    return _reg;
  }

  _AIUSolution(const std::string &body)
      : _code(AIU_RESPONSE_FAILURE),
        _context(new mlir::MLIRContext(getRegistry())) {
    _context->allowUnregisteredDialects();
    mlir::ParserConfig config(_context);
    auto moduleRef =
        mlir::parseSourceString<mlir::ModuleOp>(body.c_str(), config);
    if (moduleRef) {
      _d = moduleRef.release();
      _d.dump();
      _code = AIU_RESPONSE_SUCCESS;
    }
  }
  _AIUSolution(AIUResponseCode c) : _code(c) {
    assert(_code != AIU_RESPONSE_SUCCESS);
  }

  mlir::ModuleOp get() const { return _d; }

  AIUResponseCode getCode() const { return _code; }

private:
  AIUResponseCode _code;
  mlir::MLIRContext *_context;
  mlir::ModuleOp _d;
};

#endif /* AIUNITE_INTERNAL_SOLUTION_H */
