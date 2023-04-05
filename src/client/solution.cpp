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

#define AIU_CHECK_OBJECT(X) if (X != nullptr) ; \
  else return AIU_INVALID_OBJECT
#define AIU_GET_OBJECT(X) AIU_CHECK_OBJECT(X ## _);    \
  auto X = X ## _->_d

#define AIU_CHECK_RESULT(X) if (X != nullptr) ; \
  else return AIU_INVALID_RESULT_PTR


/******************************************************************************/
/*  SOLUTION SPEC                                                             */
/******************************************************************************/

/* -- Generated kernels and call graph */
extern "C" AIUResultCode
AIUGetObject(AIUSolution solution, AIUModel kernel, AIUBinary *result) {
  
  return AIU_FAILURE;
}
/*   - EGraph */
/*   - Binary(s) */

/* -- Tuning space */



