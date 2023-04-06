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
/*  MODEL MGMT                                                                */
/******************************************************************************/

template <typename T>
struct _AIUObject {
  _AIUObject() {}
  _AIUObject(T t) : _d(t) {}
  T get() const { return  _d; }
protected:
  T _d;
};


struct _AIUType : public _AIUObject<mlir::Type> {
  using _AIUObject::_AIUObject;
};

struct _AIUValue : public _AIUObject<mlir::Value> {
  using _AIUObject::_AIUObject;
};

struct _AIUAttr : public _AIUObject<mlir::NamedAttribute> {
  using _AIUObject::_AIUObject;
};

struct _AIUModel : public _AIUObject<mlir::func::FuncOp> {

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
    
    std::string modname = "module_";
    _module = mlir::ModuleOp::create(_loc, modname + name_);

    _file = b.getStringAttr(name_);
    
    auto funcType = b.getFunctionType({}, {});
    _d = b.create<mlir::func::FuncOp>(_loc, name_, funcType);
    _d->setAttr("kernel", b.getUnitAttr());

    _block = _d.addEntryBlock();
  
    _module.push_back(_d);
  }

  // Clone interface
  _AIUModel(mlir::func::FuncOp func)
    : _AIUModel() {
    std::string modname = "module_";
    _module = mlir::ModuleOp::create(_loc, (modname + func.getName()).str());

    // TODO: types in clone types also!!!
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

private:
  mlir::MLIRContext *_context;
  mlir::Location _loc;

  mlir::DialectRegistry &getRegistry() {
    static mlir::DialectRegistry _reg;
    _reg.insert<mlir::func::FuncDialect>(); // call once
    return _reg;
  }

  mlir::ModuleOp _module;
  mlir::Block *_block;
  int64_t _opCnt = 0;
  mlir::StringAttr _file;
  std::list<_AIUType> _types;
  std::list<_AIUAttr> _attrs;
  std::list<_AIUValue> _values;
  std::string _model_str;
};

#endif /* AIUNITE_INTERNAL_MODEL_H */
