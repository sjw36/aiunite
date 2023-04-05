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
#include <aiunite/internal/support.h>


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



