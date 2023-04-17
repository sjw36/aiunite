/******************************************************************************/
/*  CPX:   AI Unite API                                                       */
/*  Common Pluggable eXchange                                                 */
/******************************************************************************/

#include <_aiu/client/model.h>
#include <aiunite/common/types.h>

#include <mlir/Dialect/Tosa/IR/TosaOps.h>
#include <mlir/IR/BuiltinOps.h>
#include <mlir/IR/BuiltinTypes.h>

#include <list>

static mlir::DialectRegistry &getRegistry() {
  static mlir::DialectRegistry _reg;
  _reg.insert<mlir::func::FuncDialect>(); // call once
  return _reg;
}


_AIUModel::_AIUModel()
  : _context(new mlir::MLIRContext(getRegistry())),
    _loc(mlir::UnknownLoc::get(_context)), _block(nullptr), _opCnt(0) {

  // load dialects
  _context->loadDialect<mlir::func::FuncDialect, mlir::tosa::TosaDialect>();
}

  // Build interface
_AIUModel::_AIUModel(const char *name_) : _AIUModel() {
  mlir::OpBuilder b(_context);

  std::string modname = "module_";
  _module = mlir::ModuleOp::create(_loc, modname + name_);

  _file = b.getStringAttr(name_);

  auto funcType = b.getFunctionType({}, {});
  _d = b.create<mlir::func::FuncOp>(_loc, name_, funcType);
  // TODO: make conditional
  _d->setAttr("kernel", b.getUnitAttr());

  _block = _d.addEntryBlock();

  _module.push_back(_d);
}

  // Clone interface
_AIUModel::_AIUModel(mlir::func::FuncOp func) : _AIUModel() {
  std::string modname = "module_";
  _module = mlir::ModuleOp::create(_loc, (modname + func.getName()).str());

  // TODO: types in clone types also!!!
  _d = func.clone();

  _module.push_back(_d);
  // TODO: annotate ops with loc
}

_AIUModel::~_AIUModel() {
  delete _context;
}

AIUType _AIUModel::getNextType(mlir::Type t) {
  //_model_str.clear();
  _types.emplace_back(t);
  return &_types.back();
}

AIUValue _AIUModel::getNextValue(mlir::Value v) {
  //_model_str.clear();
  _values.emplace_back(v);
  return &_values.back();
}

AIUAttr _AIUModel::getNextAttr(mlir::NamedAttribute a) {
  //_model_str.clear();
  _attrs.emplace_back(a);
  return &_attrs.back();
}

mlir::Location _AIUModel::getNextLoc() {
  //_model_str.clear();
  return mlir::FileLineColLoc::get(_file, _opCnt++, 0);
}

mlir::OpBuilder _AIUModel::getBuilder() {
  //_model_str.clear();
  return mlir::OpBuilder::atBlockEnd(_block);
}

const std::string &_AIUModel::print() {
  if (_model_str.empty()) {
    llvm::raw_string_ostream os(_model_str);
    mlir::OpPrintingFlags flags;
    _module.print(os, flags.enableDebugInfo().assumeVerified());
  }
  return _model_str;
}

