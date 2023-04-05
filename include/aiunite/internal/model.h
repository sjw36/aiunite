/******************************************************************************/
/*  CPX:   AI Unite API                                                       */
/*  Common Pluggable eXchange                                                 */
/******************************************************************************/

#ifndef AIUNITE_INTERNAL_MODEL_H
#define AIUNITE_INTERNAL_MODEL_H

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

struct _AIUModel {
  mlir::DialectRegistry _reg;
  mlir::MLIRContext *_context;
  mlir::Location _loc;

  mlir::DialectRegistry &getRegistry() {
    _reg.insert<mlir::func::FuncDialect>();
    return _reg;
  }

  mlir::ModuleOp _module;
  mlir::func::FuncOp _d;
  mlir::Block *_block;
  int64_t _opCnt = 0;
  mlir::StringAttr _file;
  std::list<_AIUType> _types;
  std::list<_AIUAttr> _attrs;
  std::list<_AIUValue> _values;
  std::string _model_str;

  _AIUModel()
    : _context(new mlir::MLIRContext(getRegistry()))
    , _loc(mlir::UnknownLoc::get(_context))
    , _block(nullptr)
    , _opCnt(0) {

    // load dialects
    _context->loadDialect<mlir::func::FuncDialect,
                          mlir::tosa::TosaDialect>();
  }
  
  // Build interface
  _AIUModel(const char *name_)
    : _AIUModel() {
    mlir::OpBuilder b(_context);
    auto loc = b.getUnknownLoc();
    
    std::string modname = "module_";
    _module = mlir::ModuleOp::create(loc, modname + name_);

    _file = b.getStringAttr(name_);
    
    auto funcType = b.getFunctionType({}, {});
    _d = b.create<mlir::func::FuncOp>(loc, name_, funcType);
    _d->setAttr("kernel", b.getUnitAttr());

    _block = _d.addEntryBlock();
  
    _module.push_back(_d);
  }

  // Clone interface
  _AIUModel(mlir::func::FuncOp func)
    : _AIUModel() {
    std::string modname = "module_";
    _module = mlir::ModuleOp::create(_loc, (modname + func.getName()).str());

    _d = func.clone();

    _module.push_back(_d);
    // TODO: annotate ops with loc
  }

  ~_AIUModel() {
    delete _context;
  }
  
  AIUType getNextType(mlir::Type t) {
    //_model_str.clear();
    _types.emplace_back(t);
    return &_types.back();
  }

  AIUValue getNextValue(mlir::Value v) {
    //_model_str.clear();
    _values.emplace_back(v);
    return &_values.back();
  }

  AIUAttr getNextAttr(mlir::NamedAttribute a) {
    //_model_str.clear();
    _attrs.emplace_back(a);
    return &_attrs.back();
  }

  mlir::Location getNextLoc() {
    //_model_str.clear();
    return mlir::FileLineColLoc::get(_file, _opCnt++, 0);
  }
  mlir::OpBuilder getBuilder() {
    //_model_str.clear();
    // TODO: store
    return mlir::OpBuilder::atBlockEnd(_block);
  }

  const char *print() {
    if (_model_str.empty()) {
      llvm::raw_string_ostream os(_model_str);
      mlir::OpPrintingFlags flags;
      _module.print(os, flags.enableDebugInfo().assumeVerified());
    }
    return _model_str.c_str();
  }
};

#endif /* AIUNITE_INTERNAL_MODEL_H */
