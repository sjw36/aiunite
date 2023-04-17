/******************************************************************************/
/*  AIU:   OPEN Kernel Generator Interface                                   */
/*  AIKGI:  AI Kernel Generator Interface                                     */
/*  AIAPI:  AI Accelerator (Programming) Interface                            */
/******************************************************************************/

#include <mlir/CAPI/IR.h>

#include <aiunite/client.h>
#include <_aiu/client/model.h>
#include <_aiu/support.h>
#include <_aiu/logger.h>

#include <mlir/Dialect/Tosa/IR/TosaOps.h>

/******************************************************************************/
/*  PROBLEM SPEC                                                              */
/******************************************************************************/

extern "C" AIUResultCode AIUCreateModel(const char *name_, AIUModel *result_) {
  AIU_LOG_FUNC(AIUCreateModel);
  AIU_CHECK_OBJECT(name_);
  AIU_CHECK_RESULT(result_);

  *result_ = new _AIUModel(name_);
  return AIU_SUCCESS;
}

extern "C" AIUResultCode AIUDestroyModel(AIUModel func_) {
  AIU_LOG_FUNC(AIUDestroyModel);
  delete func_;
  return AIU_SUCCESS;
}

extern "C" AIUResultCode AIUPrintModel(AIUModel model_, const char **result_) {
  AIU_LOG_FUNC(AIUPrintModel);
  AIU_CHECK_OBJECT(model_);
  AIU_CHECK_RESULT(result_);
  *result_ = model_->print().c_str();
  return AIU_SUCCESS;
}

/*
extern "C" AIUResultCode AIUGenerateKernel(AIUContext, AIUOpType optype, ...,
AIUModel *result);
*/

/* 2. Builder Spec */

/*  - types */
/*
extern "C" AIUResultCode
AIUMakeTensorType(AIUContext, int64_t dim_cnt, int64_t dims[], AIUType elemType,
AIUType *result) { return AIU_FAILURE;
}
*/

mlir::Type getMLIRType(AIUModel model_, AIUTypeEnum elemType_) {
  mlir::OpBuilder b = model_->getBuilder();

  switch (elemType_) {
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

extern "C" AIUResultCode AIUGetType(AIUModel model_, AIUTypeEnum elemType_,
                                    AIUType *result_) {
  AIU_LOG_FUNC(AIUGetType);
  AIU_CHECK_OBJECT(model_);
  AIU_CHECK_RESULT(result_);
  if (mlir::Type etype = getMLIRType(model_, elemType_)) {
    *result_ = model_->getNextType(etype);
    return AIU_SUCCESS;
  }
  return AIU_FAILURE;
}

extern "C" AIUResultCode AIUGetTensorType(AIUModel model_, int64_t dimCnt_,
                                          int64_t dims_[],
                                          AIUTypeEnum elemType_,
                                          AIUType *result_) {
  AIU_LOG_FUNC(AIUGetTensorType);
  AIU_CHECK_OBJECT(model_);
  AIU_CHECK_RESULT(result_);
  if (mlir::Type etype = getMLIRType(model_, elemType_)) {
    llvm::ArrayRef<int64_t> shape(dims_, dimCnt_);
    mlir::Type ttype = mlir::RankedTensorType::get(shape, etype);
    *result_ = model_->getNextType(ttype);
    return AIU_SUCCESS;
  }
  return AIU_FAILURE;
}

/*  - kernel func params */
extern "C" AIUResultCode AIUAddParameter(AIUModel func_, AIUType paramType_,
                                         AIUValue *result_) {
  AIU_LOG_FUNC(AIUAddParameter);
  AIU_GET_OBJECT(func);
  AIU_CHECK_OBJECT(paramType_);
  AIU_CHECK_RESULT(result_);

  mlir::Type paramTy = paramType_->get();

  // 1) update function type
  auto funcType = func.getFunctionType();
  llvm::SmallVector<mlir::Type, 8> types(funcType.getInputs());
  types.push_back(paramTy);
  auto nFuncType =
      mlir::FunctionType::get(func.getContext(), types, funcType.getResults());
  func.setType(nFuncType);

  // 2) add to block
  mlir::Block *block = &func.getBlocks().front();
  mlir::Value param = block->addArgument(paramTy, func_->getNextLoc());
  *result_ = func_->getNextValue(param);

  return AIU_SUCCESS;
}

const char *AIUGetAttrName(AIUAttrEnum attr) {
  switch (attr) {
  case AIU_AXIS_ATTR:
    return "axis";
  case AIU_BORDER_ATTR:
    return "border";
  case AIU_CONFIG_ATTR:
    return "config";
  case AIU_DILATION_ATTR:
    return "dilation";
  case AIU_DOUBLE_ROUND_ATTR:
    return "double_round";
  case AIU_IDENTIFIER_ATTR:
    return "identifier";
  case AIU_IMPLEMENTATION_ATTRS_ATTR:
    return "implementation_attrs";
  case AIU_INPUT_ZP_ATTR:
    return "input_zp";
  case AIU_INVERSE_ATTR:
    return "inverse";
  case AIU_KERNEL_ATTR:
    return "kernel";
  case AIU_LAYOUT_ATTR:
    return "layout";
  case AIU_MAX_FP_ATTR:
    return "max_fp";
  case AIU_MAX_INT_ATTR:
    return "max_int";
  case AIU_MIN_FP_ATTR:
    return "min_fp";
  case AIU_MIN_INT_ATTR:
    return "min_int";
  case AIU_MODE_ATTR:
    return "mode";
  case AIU_MULTIPLES_ATTR:
    return "multiples";
  case AIU_MULTIPLIER_ATTR:
    return "multiplier";
  case AIU_NEW_SHAPE_ATTR:
    return "new_shape";
  case AIU_OFFSET_ATTR:
    return "offset";
  case AIU_OUT_PAD_ATTR:
    return "out_pad";
  case AIU_OUTPUT_ZP_ATTR:
    return "output_zp";
  case AIU_OUT_SHAPE_ATTR:
    return "out_shape";
  case AIU_PAD_ATTR:
    return "pad";
  case AIU_PER_CHANNEL_ATTR:
    return "per_channel";
  case AIU_CONV_QUANTIZATION_INFO_ATTR:
    return "quantization_info";
  case AIU_MATMUL_QUANTIZATION_INFO_ATTR:
    return "quantization_info";
  case AIU_PAD_QUANTIZATION_INFO_ATTR:
    return "quantization_info";
  case AIU_UNARY_QUANTIZATION_INFO_ATTR:
    return "quantization_info";
  case AIU_ROUND_ATTR:
    return "round";
  case AIU_SCALE32_ATTR:
    return "scale32";
  case AIU_SCALE_ATTR:
    return "scale";
  case AIU_SHIFT_ARRAY_ATTR:
    return "shift";
  case AIU_SHIFT_ATTR:
    return "shift";
  case AIU_SIZE_ATTR:
    return "size";
  case AIU_START_ATTR:
    return "start";
  case AIU_STRIDE_ATTR:
    return "stride";
  case AIU_VALUE_ATTR:
    return "value";
  }
  return nullptr;
}

/*  - make attribute */
extern "C" AIUResultCode AIUMakeAttr(AIUModel func_, AIUAttrEnum type_,
                                     void *value_, AIUAttr *result_) {
  AIU_LOG_FUNC(AIUMakeAttr);
  AIU_CHECK_OBJECT(func_);
  AIU_CHECK_RESULT(result_);

  const char *attr_name = AIUGetAttrName(type_);
  AIU_CHECK_OBJECT(attr_name);

  auto b = func_->getBuilder();

  mlir::Attribute attr;
  switch (type_) {
  case AIU_DOUBLE_ROUND_ATTR:
  case AIU_INVERSE_ATTR:
  case AIU_PER_CHANNEL_ATTR:
  case AIU_ROUND_ATTR:
  case AIU_SCALE32_ATTR:
    attr = b.getBoolAttr(*reinterpret_cast<int32_t *>(value_));
    break;
  case AIU_MULTIPLIER_ATTR:
  case AIU_SHIFT_ARRAY_ATTR:
    return AIU_FAILURE; // array attr
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
    return AIU_FAILURE; // array attr
  case AIU_VALUE_ATTR:
    // Only Const? which is handled by AddConstant
    // attr = ::mlir::ElementsAttr;
    assert(0);
    break;
  case AIU_MAX_FP_ATTR:
  case AIU_MIN_FP_ATTR:
    attr = b.getF32FloatAttr(*reinterpret_cast<float *>(value_));
    break;
  case AIU_AXIS_ATTR:
  case AIU_INPUT_ZP_ATTR:
  case AIU_MAX_INT_ATTR:
  case AIU_MIN_INT_ATTR:
  case AIU_OUTPUT_ZP_ATTR:
  case AIU_SHIFT_ATTR:
    attr = b.getI32IntegerAttr(*reinterpret_cast<int32_t *>(value_));
    break;
  case AIU_CONFIG_ATTR:
  case AIU_IDENTIFIER_ATTR:
  case AIU_IMPLEMENTATION_ATTRS_ATTR:
  case AIU_LAYOUT_ATTR:
  case AIU_MODE_ATTR:
    attr = b.getStringAttr(reinterpret_cast<const char *>(value_));
    break;
  case AIU_CONV_QUANTIZATION_INFO_ATTR:
    // attr = mlir::tosa::ConvOpQuantizationAttr;
  case AIU_MATMUL_QUANTIZATION_INFO_ATTR:
    // attr = mlir::tosa::MatMulOpQuantizationAttr;
  case AIU_PAD_QUANTIZATION_INFO_ATTR:
    // attr = mlir::tosa::PadOpQuantizationAttr;
  case AIU_UNARY_QUANTIZATION_INFO_ATTR:
    // attr = mlir::tosa::UnaryOpQuantizationAttr;
    assert(0); // TODO
    break;
  }

  if (attr) {
    *result_ = func_->getNextAttr(b.getNamedAttr(attr_name, attr));
    return AIU_SUCCESS;
  }

  return AIU_FAILURE;
}

template <typename T>
static llvm::ArrayRef<T> getARef(void *vals_, int64_t cnt_) {
  return llvm::ArrayRef<T>(reinterpret_cast<T *>(vals_), cnt_);
}

/*  - make attribute */
extern "C" AIUResultCode AIUMakeArrayAttr(AIUModel func_, AIUAttrEnum type_,
                                          int64_t valCnt_, void *value_,
                                          AIUAttr *result_) {
  AIU_LOG_FUNC(AIUMakeArrayAttr);
  AIU_CHECK_OBJECT(func_);
  AIU_CHECK_RESULT(result_);

  const char *attr_name = AIUGetAttrName(type_);
  AIU_CHECK_OBJECT(attr_name);

  auto b = func_->getBuilder();

  mlir::Attribute attr;
  switch (type_) {
  case AIU_MULTIPLIER_ATTR:
  case AIU_SHIFT_ARRAY_ATTR:
    attr = b.getDenseI32ArrayAttr(getARef<int32_t>(value_, valCnt_));
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
    attr = b.getDenseI64ArrayAttr(getARef<int64_t>(value_, valCnt_));
    break;
  default:
    return AIU_FAILURE; // not an array attr
  }

  if (attr) {
    *result_ = func_->getNextAttr(b.getNamedAttr(attr_name, attr));
    return AIU_SUCCESS;
  }

  return AIU_FAILURE;
}

/*  - add constant */
extern "C" AIUResultCode AIUAddConstant(AIUModel func_, AIUType resType_,
                                        void *value_, AIUValue *result_) {
  AIU_LOG_FUNC(AIUAddConstant);
  AIU_CHECK_OBJECT(func_);
  AIU_GET_OBJECT(resType);
  AIU_CHECK_RESULT(result_);

  auto loc = func_->getNextLoc();
  mlir::OpBuilder b = func_->getBuilder();

  auto resShapeType = resType.cast<mlir::ShapedType>();
  auto elemType = resShapeType.getElementType();

  mlir::Attribute attr;
  if (elemType.isF32()) {
    attr = mlir::DenseFPElementsAttr::get(
        resType, getARef<float>(value_, resShapeType.getNumElements()));
  } else if (elemType.isInteger(64)) {
    attr = mlir::DenseIntElementsAttr::get(
        resType, getARef<int64_t>(value_, resShapeType.getNumElements()));
  } else if (elemType.isInteger(32)) {
    attr = mlir::DenseIntElementsAttr::get(
        resType, getARef<int32_t>(value_, resShapeType.getNumElements()));
  } else {
    return AIU_FAILURE;
  }

  mlir::Value resVal = b.create<mlir::tosa::ConstOp>(loc, resType, attr);

  *result_ = func_->getNextValue(resVal);
  return AIU_SUCCESS;
}

/*  - add constant */
extern "C" AIUResultCode AIUAddConstantSplat(AIUModel func_, AIUType resType_,
                                             void *value_, AIUValue *result_) {
  AIU_LOG_FUNC(AIUAddConstantSplat);
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
    attr = mlir::DenseFPElementsAttr::get(resType, getARef<float>(value_, 1));
  } else if (elemType.isInteger(64)) {
    attr =
        mlir::DenseIntElementsAttr::get(resType, getARef<int64_t>(value_, 1));
  } else if (elemType.isInteger(32)) {
    attr =
        mlir::DenseIntElementsAttr::get(resType, getARef<int32_t>(value_, 1));
  } else {
    return AIU_FAILURE;
  }

  mlir::Value resVal = b.create<mlir::tosa::ConstOp>(loc, resType, attr);

  *result_ = func_->getNextValue(resVal);
  return AIU_SUCCESS;
}

/*  - add operation */
extern "C" AIUResultCode AIUAddOperation(AIUModel func_,
                                         AIUOperationEnum opType_,
                                         int64_t paramCnt_, AIUValue *params_,
                                         AIUType resType_, AIUValue *result_) {
  AIU_LOG_FUNC(AIUAddOperation);
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
    params.push_back(params_[i]->get());
  }
  mlir::Value resVal;

#define AIU_2_TOSA(X, Y)                                                       \
  case AIU_##X:                                                                \
    resVal = b.create<mlir::tosa::Y>(loc, resType_->get(), params);            \
    break

  switch (opType_) {
    AIU_2_TOSA(ABS, AbsOp);
    AIU_2_TOSA(ADD, AddOp);
    AIU_2_TOSA(SUB, SubOp);
    AIU_2_TOSA(MUL, MulOp);
    AIU_2_TOSA(DIV, DivOp);
    AIU_2_TOSA(TRANSPOSE, TransposeOp);
    AIU_2_TOSA(RECIPROCAL, ReciprocalOp);
    AIU_2_TOSA(EXP, ExpOp);
    AIU_2_TOSA(POW, PowOp);
    AIU_2_TOSA(SIGMOID, SigmoidOp);
    AIU_2_TOSA(TANH, TanhOp);
    AIU_2_TOSA(RSQRT, RsqrtOp);
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
                         int64_t paramCnt_, AIUValue *params_, int64_t attrCnt_,
                         AIUAttr *attrs_, AIUType resType_, AIUValue *result_) {
  AIU_LOG_FUNC(AIUAddOperationWithAttrs);
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
    params.push_back(params_[i]->get());
  }

  llvm::SmallVector<mlir::NamedAttribute, 4> attrs;
  for (int64_t i = 0; i < attrCnt_; ++i) {
    AIU_CHECK_OBJECT(attrs_[i]);
    attrs.push_back(attrs_[i]->get());
  }

  mlir::Value resVal;

#define AIU_2_TOSA_ATTR(X, Y)                                                  \
  case AIU_##X: {                                                              \
    auto op = b.create<mlir::tosa::Y>(loc, resType_->get(), params);           \
    op->setAttrs(attrs);                                                       \
    resVal = op.getResult();                                                   \
  } break

  switch (opType_) {
    AIU_2_TOSA_ATTR(CONV2D, Conv2DOp);
    AIU_2_TOSA_ATTR(CONV3D, Conv3DOp);
    AIU_2_TOSA_ATTR(DEPTHWISE_CONV2D, DepthwiseConv2DOp);
    AIU_2_TOSA_ATTR(MATMUL, MatMulOp);
    AIU_2_TOSA_ATTR(CLAMP, ClampOp);
    AIU_2_TOSA_ATTR(RESHAPE, ReshapeOp);
    AIU_2_TOSA_ATTR(MAX_POOL2D, MaxPool2dOp);
    AIU_2_TOSA_ATTR(AVG_POOL2D, AvgPool2dOp);
  default:
    return AIU_FAILURE;
  }

  *result_ = func_->getNextValue(resVal);
  return AIU_SUCCESS;
}

/*  - set return value */
extern "C" AIUResultCode AIUSetReturn(AIUModel func_, AIUValue retVal_) {
  AIU_LOG_FUNC(AIUSetReturn);
  AIU_GET_OBJECT(func);
  AIU_GET_OBJECT(retVal);

  auto loc = func_->getNextLoc();
  mlir::OpBuilder b = func_->getBuilder();

  b.create<mlir::func::ReturnOp>(loc, retVal);

  // 2) update function type
  auto funcType = func.getFunctionType();
  if (funcType.getResults().size() != 0) {
    return AIU_FAILURE;
  }
  auto nFuncType = mlir::FunctionType::get(
      func.getContext(), funcType.getInputs(), {retVal.getType()});
  func.setType(nFuncType);

  return AIU_SUCCESS;
}

/* 2. Clone Spec */
extern "C" AIUResultCode AIUCloneModel(MlirOperation func_, AIUModel *result_) {
  AIU_LOG_FUNC(AIUCloneModel);
  AIU_CHECK_RESULT(result_);
  auto func = llvm::dyn_cast<mlir::func::FuncOp>(*unwrap(func_));

  *result_ = new _AIUModel(func);

  // rename symbols
  // add locations
  // make large constants opaque

  return AIU_SUCCESS;
}

/* 3. Generator Spec */
extern "C" AIUResultCode AIUGenerateKernel(AIUModel, const char *options_,
                                           AIUModel *result_) {
  AIU_LOG_FUNC(AIUGenerateKernel);
  return AIU_FAILURE;
}
