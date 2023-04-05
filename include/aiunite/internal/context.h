/******************************************************************************/
/*  CPX:   AI Unite API                                                       */
/*  Common Pluggable eXchange                                                 */
/******************************************************************************/

#ifndef AIUNITE_INTERNAL_CONTEXT_H
#define AIUNITE_INTERNAL_CONTEXT_H

#include <mlir/Dialect/Func/IR/FuncOps.h>
#include <mlir/Dialect/Tosa/IR/TosaOps.h>
#include <mlir/IR/BuiltinTypes.h>
#include <mlir/IR/BuiltinOps.h>

#include <list>

/******************************************************************************/
/*  CONTEXT MGMT                                                              */
/******************************************************************************/

struct _AIUType {
  mlir::Type _d;
  _AIUType(mlir::Type t) : _d(t) {}
};

struct _AIUValue {
  mlir::Value _d;
  _AIUValue(mlir::Value v) : _d(v) {}
};
struct _AIUAttr {
  mlir::NamedAttribute _d;
  _AIUAttr(mlir::NamedAttribute a) : _d(a) {}
};

struct _AIUContext {
  mlir::DialectRegistry _reg;
  mlir::MLIRContext *_d;
  mlir::Location _loc;
  std::list<_AIUType> _types;

  mlir::DialectRegistry &getRegistry() {
    _reg.insert<mlir::func::FuncDialect>();
    return _reg;
  }

  _AIUContext() :
    _d(new mlir::MLIRContext(getRegistry())),
    _loc(mlir::UnknownLoc::get(_d))
  {
    _d->loadDialect<mlir::func::FuncDialect,
                    mlir::tosa::TosaDialect>();
  }
  ~_AIUContext() {
    delete _d;
  }

  AIUType getNextType(mlir::Type t) {
    _types.emplace_back(t);
    return &_types.back();
  }
};

struct _AIUModel {
  mlir::ModuleOp _module;
  mlir::func::FuncOp _d;
  int64_t _opCnt = 0;
  mlir::StringAttr _file;
  std::list<_AIUAttr> _attrs;
  std::list<_AIUValue> _values;
  std::string _model_print;

  _AIUModel(mlir::MLIRContext *ctx, const char *name_, AIUType returnType_)
    : _opCnt(0) {
    mlir::OpBuilder b(ctx);
    auto loc = b.getUnknownLoc();
    
    std::string modname = "module_";
    _module = mlir::ModuleOp::create(loc, modname + name_);

    _file = b.getStringAttr(name_);
    
    mlir::Type retType = b.getNoneType();
    if (returnType_ != nullptr) {
      retType = returnType_->_d;
    }

    auto funcType = b.getFunctionType({}, {retType});
    _d = b.create<mlir::func::FuncOp>(loc, name_, funcType);
    _d->setAttr("kernel", b.getUnitAttr());

    /* mlir::Block *block = */ _d.addEntryBlock();
  
    _module.push_back(_d);
  }

  _AIUModel(mlir::ModuleOp mod, mlir::func::FuncOp func)
    : _module(mod), _d(func) {
    // TODO: annotate ops with loc
  }

  AIUValue getNextValue(mlir::Value v) {
    //_model_print.clear();
    _values.emplace_back(v);
    return &_values.back();
  }

  AIUAttr getNextAttr(mlir::NamedAttribute a) {
    //_model_print.clear();
    _attrs.emplace_back(a);
    return &_attrs.back();
  }

  mlir::Location getNextLoc() {
    //_model_print.clear();
    return mlir::FileLineColLoc::get(_file, _opCnt++, 0);
  }
  mlir::OpBuilder getBuilder() {
    //_model_print.clear();
    mlir::Block *block = &_d.getBody().back();
    return mlir::OpBuilder::atBlockEnd(block);
  }

  const char *print() {
    if (_model_print.empty()) {
      llvm::raw_string_ostream os(_model_print);
      mlir::OpPrintingFlags flags;
      _module.print(os, flags.enableDebugInfo().assumeVerified());
    }
    return _model_print.c_str();
  }
};

#endif /* AIUNITE_INTERNAL_CONTEXT_H */

