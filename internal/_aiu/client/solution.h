/******************************************************************************/
/*  CPX:   AI Unite API                                                       */
/*  Common Pluggable eXchange                                                 */
/******************************************************************************/

#ifndef _AIU_CLIENT_SOLUTION_H
#define _AIU_CLIENT_SOLUTION_H

#include <aiunite/common/protocol.h>
#include <aiunite/common/types.h>

#include <mlir/IR/BuiltinOps.h>

#include <unordered_map>
#include <string>

struct _AIUSolution {

  _AIUSolution(AIUDevice dev, const std::string &body);

  mlir::ModuleOp get() const { return _d; }

  AIUSolutionCode getCode() const { return _code; }

  AIUBinary getBinary(const char *func_name_);

private:
  AIUSolutionCode _code;
  AIUDevice _device;
  mlir::MLIRContext *_context;
  mlir::ModuleOp _d;
  std::unordered_map<std::string, AIUBinary> _binaries;
};

#endif /* _AIU_CLIENT_SOLUTION_H */
