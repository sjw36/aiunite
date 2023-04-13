/******************************************************************************/
/*  CPX:   AI Unite API                                                       */
/*  Common Pluggable eXchange                                                 */
/******************************************************************************/

#ifndef _AIU_CLIENT_SOLUTION_H
#define _AIU_CLIENT_SOLUTION_H

#include <aiunite/common/types.h>

#include <mlir/IR/BuiltinOps.h>

struct _AIUSolution {

  _AIUSolution(const std::string &body);
  _AIUSolution(AIUResponseCode c);

  mlir::ModuleOp get() const { return _d; }

  AIUResponseCode getCode() const { return _code; }

private:
  AIUDevice _device;
  AIUResponseCode _code;
  mlir::MLIRContext *_context;
  mlir::ModuleOp _d;
};

#endif /* _AIU_CLIENT_SOLUTION_H */
