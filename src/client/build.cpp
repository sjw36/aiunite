/******************************************************************************/
/*  AIU:   OPEN Kernel Generator Interface                                   */
/*  AIKGI:  AI Kernel Generator Interface                                     */
/*  AIAPI:  AI Accelerator (Programming) Interface                            */
/******************************************************************************/

#include <mlir/CAPI/IR.h>
#include <mlir/Dialect/Func/IR/FuncOps.h>
#include <mlir/Dialect/Tosa/IR/TosaOps.h>
#include <mlir/IR/BuiltinOps.h>

#include <aiunite/client.h>

#include <list>

#define AIU_CHECK_OBJECT(X) if (X != nullptr) ; \
  else return AIU_INVALID_OBJECT
#define AIU_GET_OBJECT(X) AIU_CHECK_OBJECT(X ## _);    \
  auto X = X ## _->_d

#define AIU_CHECK_RESULT(X) if (X != nullptr || *X != nullptr) ; \
  else return AIU_INVALID_RESULT_PTR


/******************************************************************************/
/*  REGISTRY MGMT                                                             */
/******************************************************************************/

extern "C" AIUResultCode AIUReadRegistry(const char *filename) {
  return AIU_FAILURE;
}

/******************************************************************************/
/*  CONTEXT MGMT                                                              */
/******************************************************************************/
struct _AIUType {
  mlir::Type _d;
  _AIUType(mlir::Type t) : _d(t) {}
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

extern "C" AIUResultCode AIUCreateContext(AIUContext *result) {
  AIU_CHECK_RESULT(result);  

  *result = new _AIUContext;
  return AIU_SUCCESS;
}

extern "C" AIUResultCode AIUDestroyContext(AIUContext context) {
  AIU_CHECK_OBJECT(context);  
  delete context;
  return AIU_SUCCESS;
}

/******************************************************************************/
/*  PROBLEM SPEC                                                              */
/******************************************************************************/

struct _AIUValue {
  mlir::Value _d;
  _AIUValue(mlir::Value v) : _d(v) {}
};
struct _AIUAttribute {
  mlir::NamedAttribute _d;
  _AIUAttribute(mlir::NamedAttribute a) : _d(a) {}
};

struct _AIUModel {
  mlir::ModuleOp _module;
  mlir::func::FuncOp _d;
  int64_t _opCnt = 0;
  mlir::StringAttr _file;
  std::list<_AIUAttribute> _attrs;
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

  AIUAttribute getNextAttr(mlir::NamedAttribute a) {
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

extern "C" AIUResultCode AIUPrintModel(AIUModel model_, const char **result_) {
  AIU_CHECK_OBJECT(model_);
  *result_ = model_->print();
  return AIU_SUCCESS;
}

extern "C" AIUResultCode AIUCreateModel(AIUContext context_, const char *name_,
                                        AIUType returnType_, AIUModel *result_) {
  AIU_CHECK_OBJECT(context_);
  AIU_CHECK_OBJECT(name_);
  AIU_CHECK_RESULT(result_);

  *result_ = new _AIUModel(context_->_d, name_, returnType_);
  
  return AIU_SUCCESS;
}

extern "C" AIUResultCode AIUDestroyModel(AIUModel func) {
  delete func;
  return AIU_SUCCESS;
}

/* 1. Generator Spec */
extern "C" AIUResultCode AIUGenerateKernel(AIUContext, const char *options,
                                           AIUModel *result) {
  return AIU_FAILURE;
}

/*
extern "C" AIUResultCode AIUGenerateKernel(AIUContext, AIUOpType optype, ...,
AIUModel *result);
*/

/* 2. Builder Spec */

/*  - types */
/*
extern "C" AIUResultCode
AIUMakeTensorType(AIUContext, int64_t dim_cnt, int64_t dims[], AIUType elemType, AIUType *result) {
  return AIU_FAILURE;
}
*/

mlir::Type getMLIRType(AIUContext context, AIUTypeEnum elemType) {
  mlir::Builder b(context->_d);
  
  switch (elemType) {
  case AIU_F64:
    return b.getF64Type();
  case AIU_F32:
    return b.getF32Type();
  case AIU_F16:
    return b.getF16Type();
  case AIU_BF16:
    return b.getBF16Type();
  case AIU_F8:
    return b.getFloat8E4M3FNUZType();
  case AIU_BF8:
    return b.getFloat8E5M2FNUZType();
  case AIU_I32:
    return b.getI32Type();
  case AIU_I16:
    return b.getI16Type();
  case AIU_I8:
    return b.getI8Type();
  case AIU_I4:
    return b.getI4Type();
  }
  return b.getNoneType();
}


extern "C" AIUResultCode
AIUGetType(AIUContext context_, AIUTypeEnum elemType_, AIUType *result_) {
  AIU_CHECK_OBJECT(context_);
  AIU_CHECK_RESULT(result_);
  if (mlir::Type etype = getMLIRType(context_, elemType_)) {
    *result_ = context_->getNextType(etype);
    return AIU_SUCCESS;
  }
  return AIU_FAILURE;
}

extern "C" AIUResultCode
AIUGetTensorType(AIUContext context_, int64_t dimCnt_, int64_t dims_[],
                 AIUTypeEnum elemType_, AIUType *result_) {
  AIU_CHECK_OBJECT(context_);
  AIU_CHECK_RESULT(result_);
  if (mlir::Type etype = getMLIRType(context_, elemType_)) {
    llvm::ArrayRef<int64_t> shape(dims_, dimCnt_);
    mlir::Type ttype = mlir::RankedTensorType::get(shape, etype);
    *result_ = context_->getNextType(ttype);
    return AIU_SUCCESS;
  }
  return AIU_FAILURE;
}

/*  - kernel func params */
extern "C" AIUResultCode
AIUAddParameter(AIUModel func_, AIUType paramType_, AIUValue *result_) {
  AIU_GET_OBJECT(func);
  AIU_CHECK_OBJECT(paramType_);
  AIU_CHECK_RESULT(result_);

  mlir::Type paramTy = paramType_->_d;

  // 1) update function type
  auto funcType = func.getFunctionType();
  llvm::SmallVector<mlir::Type, 8> types(funcType.getInputs());
  types.push_back(paramTy);
  auto nFuncType = mlir::FunctionType::get(func.getContext(), types,
                                           funcType.getResults());
  func.setType(nFuncType);

  // 2) add to block
  mlir::Block *block = &func.getBlocks().front();
  mlir::Value param = block->addArgument(paramTy, func_->getNextLoc());
  *result_ = func_->getNextValue(param);
  
  return AIU_SUCCESS;
}

const char *AIUGetAttributeName(AIUAttributeEnum attr) {
  switch (attr) {
  case AIU_AXIS_ATTR: return "axis";
  case AIU_BORDER_ATTR: return "border";
  case AIU_CONFIG_ATTR: return "config";
  case AIU_DILATION_ATTR: return "dilation";
  case AIU_DOUBLE_ROUND_ATTR: return "double_round";
  case AIU_IDENTIFIER_ATTR: return "identifier";
  case AIU_IMPLEMENTATION_ATTRS_ATTR: return "implementation_attrs";
  case AIU_INPUT_ZP_ATTR: return "input_zp";
  case AIU_INVERSE_ATTR: return "inverse";
  case AIU_KERNEL_ATTR: return "kernel";
  case AIU_MAX_FP_ATTR: return "max_fp";
  case AIU_MAX_INT_ATTR: return "max_int";
  case AIU_MIN_FP_ATTR: return "min_fp";
  case AIU_MIN_INT_ATTR: return "min_int";
  case AIU_MODE_ATTR: return "mode";
  case AIU_MULTIPLES_ATTR: return "multiples";
  case AIU_MULTIPLIER_ATTR: return "multiplier";
  case AIU_NEW_SHAPE_ATTR: return "new_shape";
  case AIU_OFFSET_ATTR: return "offset";
  case AIU_OUT_PAD_ATTR: return "out_pad";
  case AIU_OUTPUT_ZP_ATTR: return "output_zp";
  case AIU_OUT_SHAPE_ATTR: return "out_shape";
  case AIU_PAD_ATTR: return "pad";
  case AIU_PER_CHANNEL_ATTR: return "per_channel";
  case AIU_CONV_QUANTIZATION_INFO_ATTR: return "quantization_info";
  case AIU_MATMUL_QUANTIZATION_INFO_ATTR: return "quantization_info";
  case AIU_PAD_QUANTIZATION_INFO_ATTR: return "quantization_info";
  case AIU_UNARY_QUANTIZATION_INFO_ATTR: return "quantization_info";
  case AIU_ROUND_ATTR: return "round";
  case AIU_SCALE32_ATTR: return "scale32";
  case AIU_SCALE_ATTR: return "scale";
  case AIU_SHIFT_ARRAY_ATTR: return "shift";
  case AIU_SHIFT_ATTR: return "shift";
  case AIU_SIZE_ATTR: return "size";
  case AIU_START_ATTR: return "start";
  case AIU_STRIDE_ATTR: return "stride";
  case AIU_VALUE_ATTR: return "value";
  }
  return nullptr;
}


/*  - make attribute */
extern "C" AIUResultCode
AIUMakeAttribute(AIUModel func_, AIUAttributeEnum type_, void *value_,
                 AIUAttribute *result_) {
  AIU_CHECK_OBJECT(func_);
  AIU_CHECK_RESULT(result_);

  const char *attr_name = AIUGetAttributeName(type_);
  AIU_CHECK_OBJECT(attr_name);

  auto b = func_->getBuilder();

  mlir::Attribute attr;
  switch (type_) {
  case AIU_DOUBLE_ROUND_ATTR:
  case AIU_INVERSE_ATTR:
  case AIU_PER_CHANNEL_ATTR:
  case AIU_ROUND_ATTR:
  case AIU_SCALE32_ATTR:
    attr = b.getBoolAttr(*reinterpret_cast<int32_t*>(value_));
    break;
  case AIU_MULTIPLIER_ATTR:
  case AIU_SHIFT_ARRAY_ATTR:
    //attr = ::mlir::DenseI32ArrayAttr;
    break;
  case AIU_BORDER_ATTR:
  case AIU_DILATION_ATTR:
  case AIU_KERNEL_ATTR:
  case AIU_MULTIPLES_ATTR:
  case AIU_NEW_SHAPE_ATTR:
  case AIU_OFFSET_ATTR:
  case AIU_OUT_PAD_ATTR:
  case AIU_OUT_SHAPE_ATTR:
  case AIU_PAD_ATTR:
  case AIU_SCALE_ATTR:
  case AIU_SIZE_ATTR:
  case AIU_START_ATTR:
  case AIU_STRIDE_ATTR:
    //attr = ::mlir::DenseI64ArrayAttr;
    break;
  case AIU_VALUE_ATTR:
    //attr = ::mlir::ElementsAttr;
    assert(0);
    break;
  case AIU_MAX_FP_ATTR:
  case AIU_MIN_FP_ATTR:
    attr = b.getF32FloatAttr(*reinterpret_cast<float*>(value_));
    break;
  case AIU_AXIS_ATTR:
  case AIU_INPUT_ZP_ATTR:
  case AIU_MAX_INT_ATTR:
  case AIU_MIN_INT_ATTR:
  case AIU_OUTPUT_ZP_ATTR:
  case AIU_SHIFT_ATTR:
    attr = b.getI32IntegerAttr(*reinterpret_cast<int32_t*>(value_));
    break;
  case AIU_CONFIG_ATTR:
  case AIU_IDENTIFIER_ATTR:
  case AIU_IMPLEMENTATION_ATTRS_ATTR:
  case AIU_MODE_ATTR:
    attr = b.getStringAttr(reinterpret_cast<const char*>(value_));
    break;
  case AIU_CONV_QUANTIZATION_INFO_ATTR:
    //attr = mlir::tosa::ConvOpQuantizationAttr;
    break;
  case AIU_MATMUL_QUANTIZATION_INFO_ATTR:
    //attr = mlir::tosa::MatMulOpQuantizationAttr;
    break;
  case AIU_PAD_QUANTIZATION_INFO_ATTR:
    //attr = mlir::tosa::PadOpQuantizationAttr;
    break;
  case AIU_UNARY_QUANTIZATION_INFO_ATTR:
    //attr = mlir::tosa::UnaryOpQuantizationAttr;
    break;
  }

  if (attr) {
    *result_ = func_->getNextAttr(b.getNamedAttr(attr_name, attr));
    return AIU_SUCCESS;
  }
  
  return AIU_FAILURE;
}

/*  - add constant */
extern "C" AIUResultCode
AIUAddConstant(AIUModel func_, AIUType resType_, void *value_,
               AIUValue *result_) {
  AIU_CHECK_OBJECT(func_);
  AIU_GET_OBJECT(resType);
  AIU_CHECK_RESULT(result_);

  auto loc = func_->getNextLoc();
  mlir::OpBuilder b = func_->getBuilder();

  auto resShapeType = resType.cast<mlir::ShapedType>();
  auto elemType = resShapeType.getElementType();

  mlir::Attribute attr;
  if (elemType.isF32()) {
    llvm::ArrayRef<float> vals(reinterpret_cast<float*>(value_),
                                  resShapeType.getNumElements());
    attr = mlir::DenseFPElementsAttr::get(resType, vals);
  } else if (elemType.isInteger(64)) {
    llvm::ArrayRef<int64_t> vals(reinterpret_cast<int64_t*>(value_),
                                  resShapeType.getNumElements());
    attr = mlir::DenseIntElementsAttr::get(resType, vals);
  } else if (elemType.isInteger(32)) {
    llvm::ArrayRef<int32_t> vals(reinterpret_cast<int32_t*>(value_),
                                  resShapeType.getNumElements());
    attr = mlir::DenseIntElementsAttr::get(resType, vals);
  } else {
    return AIU_FAILURE;
  }

  mlir::Value resVal = b.create<mlir::tosa::ConstOp>(loc, resType, attr);

  *result_ = func_->getNextValue(resVal);
  return AIU_SUCCESS;
}

/*  - add constant */
extern "C" AIUResultCode
AIUAddConstantSplat(AIUModel func_, AIUType resType_, void *value_,
                    AIUValue *result_) {
  AIU_GET_OBJECT(func);
  AIU_GET_OBJECT(resType);
  AIU_CHECK_RESULT(result_);

  auto loc = func_->getNextLoc();
  mlir::Block *block = &func.getBody().back();
  mlir::OpBuilder b = mlir::OpBuilder::atBlockEnd(block);

  auto resShapeType = resType.cast<mlir::ShapedType>();
  auto elemType = resShapeType.getElementType();

  mlir::Attribute attr;
  if (elemType.isF32()) {
    llvm::ArrayRef<float> vals(reinterpret_cast<float*>(value_), 1);
    attr = mlir::DenseFPElementsAttr::get(resType, vals);
  } else if (elemType.isInteger(64)) {
    llvm::ArrayRef<int64_t> vals(reinterpret_cast<int64_t*>(value_), 1);
    attr = mlir::DenseIntElementsAttr::get(resType, vals);
  } else if (elemType.isInteger(32)) {
    llvm::ArrayRef<int32_t> vals(reinterpret_cast<int32_t*>(value_), 1);
    attr = mlir::DenseIntElementsAttr::get(resType, vals);
  } else {
    return AIU_FAILURE;
  }

  mlir::Value resVal = b.create<mlir::tosa::ConstOp>(loc, resType, attr);

  *result_ = func_->getNextValue(resVal);
  return AIU_SUCCESS;
}

/*  - add operation */
extern "C" AIUResultCode
AIUAddOperation(AIUModel func_, AIUOperationEnum opType_, int64_t paramCnt_,
                AIUValue *params_, AIUType resType_, AIUValue *result_) {
  AIU_CHECK_OBJECT(func_);
  AIU_CHECK_OBJECT(resType_);
  AIU_CHECK_RESULT(result_);
  if (paramCnt_ != 0 && params_ == nullptr) {
    return AIU_FAILURE;
  }

  auto loc = func_->getNextLoc();
  mlir::OpBuilder b = func_->getBuilder();
  
  llvm::SmallVector<mlir::Value, 4> params;
  for (int64_t i = 0; i < paramCnt_; ++i) {
    AIU_CHECK_OBJECT(params_[i]);
    params.push_back(params_[i]->_d);
  }
  mlir::Value resVal;

#define AIU_2_TOSA(X, Y) case AIU_ ## X: resVal = b.create<mlir::tosa::Y>(loc, resType_->_d, params); break
  
  switch (opType_) {
    AIU_2_TOSA(ADD, AddOp);
    AIU_2_TOSA(MUL, MulOp);
    default:
      return AIU_FAILURE; // unknown op
      break;
  }
  *result_ = func_->getNextValue(resVal);
  
  return AIU_SUCCESS;
}

/*  - add operation */
extern "C" AIUResultCode
AIUAddOperationWithAttrs(AIUModel func_, AIUOperationEnum opType_,
                         int64_t paramCnt_, AIUValue *params_,
                         int64_t attrCnt_, AIUAttribute *attrs_,
                         AIUType resType_, AIUValue *result_) {
  AIU_CHECK_OBJECT(func_);
  AIU_CHECK_OBJECT(resType_);
  AIU_CHECK_RESULT(result_);
  if (paramCnt_ != 0 && params_ == nullptr) {
    return AIU_FAILURE;
  }
  if (attrCnt_ != 0 && attrs_ == nullptr) {
    return AIU_FAILURE;
  }

  auto loc = func_->getNextLoc();
  mlir::OpBuilder b = func_->getBuilder();
  
  llvm::SmallVector<mlir::Value, 4> params;
  for (int64_t i = 0; i < paramCnt_; ++i) {
    AIU_CHECK_OBJECT(params_[i]);
    params.push_back(params_[i]->_d);
  }
  mlir::Value resVal;

#define AIU_2_TOSA(X, Y) case AIU_ ## X: resVal = b.create<mlir::tosa::Y>(loc, resType_->_d, params); break
  
  switch (opType_) {
    AIU_2_TOSA(CONV2D, Conv2DOp);
    default: break;
  }
  *result_ = func_->getNextValue(resVal);
  return AIU_SUCCESS;
}

/*  - set return value */
extern "C" AIUResultCode
AIUSetReturn(AIUModel func_, AIUValue retVal_) {
  AIU_CHECK_OBJECT(func_);
  AIU_GET_OBJECT(retVal);
  // check func return type vs return value

  auto loc = func_->getNextLoc();
  mlir::OpBuilder b = func_->getBuilder();
  b.create<mlir::func::ReturnOp>(loc, retVal);

  return AIU_SUCCESS;
}


/* 3. Clone Spec */
extern "C" AIUResultCode AIUCloneModel(AIUContext context_, MlirOperation func_,
                                       AIUModel *result_) {
  AIU_CHECK_OBJECT(context_);
  AIU_CHECK_RESULT(result_);
  auto func = llvm::dyn_cast<mlir::func::FuncOp>(*unwrap(func_));

  std::string modname = "module_";
  auto mod =
      mlir::ModuleOp::create(context_->_loc, (modname + func.getName()).str());

  mod.push_back(func.clone());

  // rename symbols
  // strip out locations
  // make large constants opaque

  *result_ = new _AIUModel(mod, func);
  return AIU_SUCCESS;
}
