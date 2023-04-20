/******************************************************************************/
/*  CPX:   AI ComPleX API                                                     */
/*  Common Pluggable eXchange                                                 */
/******************************************************************************/

#ifndef AIUNITE_CLIENT_BUILD_H
#define AIUNITE_CLIENT_BUILD_H

#include <aiunite/common/protocol.h>
#include <aiunite/common/types.h>

#include <mlir-c/IR.h>

/******************************************************************************/
/*  PROBLEM SPEC                                                              */
/******************************************************************************/

extern "C" AIUResultCode AIUCreateModel(const char *name_, AIUModel *result_);

extern "C" AIUResultCode AIUDestroyModel(AIUModel);

extern "C" AIUResultCode AIUPrintModel(AIUModel model_, const char **result_);

/* 1. Builder Spec */

enum AIUTypeEnum {
  AIU_F64,
  AIU_F32,
  AIU_F16,
  AIU_BF16,
  AIU_F8,
  AIU_BF8,
  AIU_I32,
  AIU_I16,
  AIU_I8,
  AIU_I4
};

enum AIUOperationEnum {
  AIU_ABS,
  AIU_ADD,
  AIU_APPLY_SCALE,
  AIU_ARGMAX,
  AIU_ARITHMETIC_RIGHT_SHIFT,
  AIU_AVG_POOL2D,
  AIU_BITWISE_AND,
  AIU_BITWISE_NOT,
  AIU_BITWISE_OR,
  AIU_BITWISE_XOR,
  AIU_CAST,
  AIU_CEIL,
  AIU_CLAMP,
  AIU_CLZ,
  AIU_CONCAT,
  AIU_CONST,
  AIU_CONV2D,
  AIU_CONV3D,
  AIU_CUSTOM,
  AIU_DEPTHWISE_CONV2D,
  AIU_DIV,
  AIU_EQUAL,
  AIU_EXP,
  AIU_FFT2D,
  AIU_FLOOR,
  AIU_FULLY_CONNECTED,
  AIU_GATHER,
  AIU_GREATER_EQUAL,
  AIU_GREATER,
  AIU_IDENTITY,
  AIU_COND_IF,
  AIU_LOG,
  AIU_LOGICAL_AND,
  AIU_LOGICAL_LEFT_SHIFT,
  AIU_LOGICAL_NOT,
  AIU_LOGICAL_OR,
  AIU_LOGICAL_RIGHT_SHIFT,
  AIU_LOGICAL_XOR,
  AIU_MATMUL,
  AIU_MAX_POOL2D,
  AIU_MAXIMUM,
  AIU_MINIMUM,
  AIU_MUL,
  AIU_NEGATE,
  AIU_PAD,
  AIU_POW,
  AIU_RFFT2D,
  AIU_RECIPROCAL,
  AIU_REDUCE_ALL,
  AIU_REDUCE_ANY,
  AIU_REDUCE_MAX,
  AIU_REDUCE_MIN,
  AIU_REDUCE_PROD,
  AIU_REDUCE_SUM,
  AIU_RESCALE,
  AIU_RESHAPE,
  AIU_RESIZE,
  AIU_REVERSE,
  AIU_RSQRT,
  AIU_SCATTER,
  AIU_SELECT,
  AIU_SIGMOID,
  AIU_SLICE,
  AIU_SUB,
  AIU_TABLE,
  AIU_TANH,
  AIU_TILE,
  AIU_TRANSPOSE_CONV2D,
  AIU_TRANSPOSE,
};

enum AIUAttrEnum {
  AIU_AXIS_ATTR,                     /* ::mlir::IntegerAttr */
  AIU_BORDER_ATTR,                   /* ::mlir::DenseI64ArrayAttr */
  AIU_CONFIG_ATTR,                   /* ::mlir::StringAttr */
  AIU_DILATION_ATTR,                 /* ::mlir::DenseI64ArrayAttr */
  AIU_DOUBLE_ROUND_ATTR,             /* ::mlir::BoolAttr */
  AIU_IDENTIFIER_ATTR,               /* ::mlir::StringAttr */
  AIU_IMPLEMENTATION_ATTRS_ATTR,     /* ::mlir::StringAttr */
  AIU_INPUT_ZP_ATTR,                 /* ::mlir::IntegerAttr */
  AIU_INVERSE_ATTR,                  /* ::mlir::BoolAttr */
  AIU_KERNEL_ATTR,                   /* ::mlir::DenseI64ArrayAttr */
  AIU_LAYOUT_ATTR,                   /* ::mlir::StringAttr */
  AIU_MAX_FP_ATTR,                   /* ::mlir::FloatAttr */
  AIU_MAX_INT_ATTR,                  /* ::mlir::IntegerAttr */
  AIU_MIN_FP_ATTR,                   /* ::mlir::FloatAttr */
  AIU_MIN_INT_ATTR,                  /* ::mlir::IntegerAttr */
  AIU_MODE_ATTR,                     /* ::mlir::StringAttr */
  AIU_MULTIPLES_ATTR,                /* ::mlir::DenseI64ArrayAttr */
  AIU_MULTIPLIER_ATTR,               /* ::mlir::DenseI32ArrayAttr */
  AIU_NEW_SHAPE_ATTR,                /* ::mlir::DenseI64ArrayAttr */
  AIU_OFFSET_ATTR,                   /* ::mlir::DenseI64ArrayAttr */
  AIU_OUT_PAD_ATTR,                  /* ::mlir::DenseI64ArrayAttr */
  AIU_OUTPUT_ZP_ATTR,                /* ::mlir::IntegerAttr */
  AIU_OUT_SHAPE_ATTR,                /* ::mlir::DenseI64ArrayAttr */
  AIU_PAD_ATTR,                      /* ::mlir::DenseI64ArrayAttr */
  AIU_PER_CHANNEL_ATTR,              /* ::mlir::BoolAttr */
  AIU_CONV_QUANTIZATION_INFO_ATTR,   /* mlir::tosa::ConvOpQuantizationAttr */
  AIU_MATMUL_QUANTIZATION_INFO_ATTR, /* mlir::tosa::MatMulOpQuantizationAttr */
  AIU_PAD_QUANTIZATION_INFO_ATTR,    /* mlir::tosa::PadOpQuantizationAttr */
  AIU_UNARY_QUANTIZATION_INFO_ATTR,  /* mlir::tosa::UnaryOpQuantizationAttr */
  AIU_ROUND_ATTR,                    /* ::mlir::BoolAttr */
  AIU_SCALE32_ATTR,                  /* ::mlir::BoolAttr */
  AIU_SCALE_ATTR,                    /* ::mlir::DenseI64ArrayAttr */
  AIU_SHIFT_ARRAY_ATTR,              /* ::mlir::DenseI32ArrayAttr */
  AIU_SHIFT_ATTR,                    /* ::mlir::IntegerAttr */
  AIU_SIZE_ATTR,                     /* ::mlir::DenseI64ArrayAttr */
  AIU_START_ATTR,                    /* ::mlir::DenseI64ArrayAttr */
  AIU_STRIDE_ATTR,                   /* ::mlir::DenseI64ArrayAttr */
  AIU_VALUE_ATTR                     /* ::mlir::ElementsAttr */
};

/*  - types */
extern "C" AIUResultCode AIUGetType(AIUModel, AIUTypeEnum elemType,
                                    AIUType *result);

extern "C" AIUResultCode AIUGetTensorType(AIUModel, int64_t dim_cnt,
                                          const int64_t dims[], AIUTypeEnum elemType,
                                          AIUType *result);

/*  - kernel func params */
extern "C" AIUResultCode AIUAddParameter(AIUModel func, AIUType type,
                                         AIUValue *result);

/*  - make attribute */
extern "C" AIUResultCode AIUMakeAttr(AIUModel func, AIUAttrEnum type,
                                     const void *value, AIUAttr *result);

/*  - make attribute */
extern "C" AIUResultCode AIUMakeArrayAttr(AIUModel func, AIUAttrEnum type,
                                          int64_t valCnt, const void *value,
                                          AIUAttr *result);

/*  - add constant */
extern "C" AIUResultCode AIUAddConstant(AIUModel func, AIUType resType,
                                        const void *value, AIUValue *result);

/*  - add constant */
extern "C" AIUResultCode AIUAddConstantSplat(AIUModel func, AIUType resType,
                                             const void *value, AIUValue *result);

/*  - add operation */
extern "C" AIUResultCode AIUAddOperation(AIUModel func, AIUOperationEnum type,
                                         int64_t paramCnt, const AIUValue *params,
                                         AIUType resType, AIUValue *result);

/*  - add operation */
extern "C" AIUResultCode
AIUAddOperationWithAttrs(AIUModel func, AIUOperationEnum type, int64_t paramCnt,
                         const AIUValue *params, int64_t attrCnt, const AIUAttr *attrs,
                         AIUType resType, AIUValue *result);

/*  - set return value */
extern "C" AIUResultCode AIUSetReturn(AIUModel func, AIUValue retVals);

/* 2. Clone Spec */
extern "C" AIUResultCode AIUCloneModel(MlirOperation kernel, AIUModel *result);

/* 3. Generator Spec */
extern "C" AIUResultCode AIUGenerateModel(const char *options,
                                          AIUModel *result);

#endif /* AIUNITE_CLIENT_BUILD_H */
