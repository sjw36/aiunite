/******************************************************************************/
/*  CPX:   AI Unite API                                                       */
/*  Common Pluggable eXchange                                                 */
/******************************************************************************/

#ifndef AIUNITE_INTERNAL_SOLUTION_H
#define AIUNITE_INTERNAL_SOLUTION_H

#include <mlir/Dialect/Func/IR/FuncOps.h>
#include <mlir/Dialect/Tosa/IR/TosaOps.h>

struct _AIUSolution {
  mlir::MLIRContext *_context;

  mlir::DialectRegistry &getRegistry() {
    static mlir::DialectRegistry _reg;
    // call_once
    _reg.insert<mlir::func::FuncDialect>();
    return _reg;
  }

  mlir::ModuleOp _module;

  _AIUSolution(const std::string &body)
    : _context(new mlir::MLIRContext(getRegistry()))
  {
    _context->allowUnregisteredDialects();
    mlir::ParserConfig config(_context);
    auto moduleRef = mlir::parseSourceString<mlir::ModuleOp>(
                         body.c_str(), config);
    _module = moduleRef.release();
    _module.dump();
  }
};


#endif /* AIUNITE_INTERNAL_SOLUTION_H */
