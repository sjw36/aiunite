/******************************************************************************/
/*  CPX:   AI Unite API                                                       */
/*  Common Pluggable eXchange                                                 */
/******************************************************************************/

#ifndef _AIU_CLIENT_MODEL_H
#define _AIU_CLIENT_MODEL_H

#include <aiunite/common/types.h>

#include <mlir/Dialect/Func/IR/FuncOps.h>
#include <mlir/IR/BuiltinOps.h>
//#include <mlir/IR/BuiltinTypes.h>

#include <list>

/******************************************************************************/
/*  MODEL MGMT                                                                */
/******************************************************************************/

template <typename T> struct _AIUObject {
  _AIUObject() {}
  _AIUObject(T t) : _d(t) {}
  T get() const { return _d; }

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

  _AIUModel();
  // Build interface
  _AIUModel(const char *name_);
  // Clone interface
  _AIUModel(mlir::func::FuncOp func);
  ~_AIUModel();

  AIUType getNextType(mlir::Type t);
  AIUValue getNextValue(mlir::Value v);
  AIUAttr getNextAttr(mlir::NamedAttribute a);

  mlir::Location getNextLoc();
  mlir::OpBuilder getBuilder();

  const std::string &print();

private:
  mlir::MLIRContext *_context;
  mlir::Location _loc;

  mlir::ModuleOp _module;
  mlir::Block *_block;
  int64_t _opCnt = 0;
  mlir::StringAttr _file;
  std::list<_AIUType> _types;
  std::list<_AIUAttr> _attrs;
  std::list<_AIUValue> _values;
  std::string _model_str;
};

#endif /* _AIU_CLIENT_MODEL_H */
