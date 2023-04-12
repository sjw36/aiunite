/******************************************************************************/
/*  AIU:   OPEN Kernel Generator Interface                                   */
/*  AIKGI:  AI Kernel Generator Interface                                     */
/*  AIAPI:  AI Accelerator (Programming) Interface                            */
/******************************************************************************/

#include <mlir/CAPI/IR.h>
#include <mlir/Dialect/Func/IR/FuncOps.h>
#include <mlir/Dialect/Tosa/IR/TosaOps.h>
#include <mlir/Dialect/XModel/IR/XModel.h>
#include <mlir/IR/BuiltinOps.h>

#include <aiunite/client.h>
#include <_aiu/client/solution.h>
#include <_aiu/support.h>
#include <_aiu/logger.h>

/******************************************************************************/
/*  SOLUTION SPEC                                                             */
/******************************************************************************/
extern "C" AIUResponseCode AIUGetResponseCode(AIUSolution solution_) {
  AIU_LOG_FUNC(AIUGetResponseCode);
  AIU_CHECK_OBJECT(solution_);
  return solution_->getCode();
}

/* -- Generated kernels and call graph */
extern "C" MlirModule AIUGetModule(AIUSolution solution_) {
  AIU_LOG_FUNC(AIUGetModule);
  assert(solution_ != nullptr);
  return wrap(solution_->get());
}

struct _AIUBinary {

private:
};

/*   - Binary(s) */
extern "C" AIUResultCode AIUGetBinary(AIUSolution solution_,
                                      const char *kernel_name_,
                                      AIUBinary *result_) {
  AIU_LOG_FUNC(AIUGetBinary);
  AIU_CHECK_OBJECT(solution_);
  AIU_CHECK_RESULT(result_);

  return AIU_SUCCESS;
}

extern "C" AIUResultCode AIUGetObject(AIUBinary bin_, char *buffer_,
                                      size_t *size_) {
  AIU_LOG_FUNC(AIUGetObject);

  return AIU_FAILURE;
}

extern "C" AIUResultCode AIUGetLaunchDims(AIUBinary bin_, size_t *global_size_,
                                          size_t *local_size_) {
  AIU_LOG_FUNC(AIUGetLaunchDims);

  return AIU_FAILURE;
}

/* -- Tuning space */
