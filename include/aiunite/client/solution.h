/******************************************************************************/
/*  CPX:   AI ComPleX API                                                     */
/*  Common Pluggable eXchange                                                 */
/******************************************************************************/

#ifndef AIUNITE_CLIENT_SOLUTION_H
#define AIUNITE_CLIENT_SOLUTION_H

#include <aiunite/common/protocol.h>
#include <aiunite/common/types.h>
#include <mlir-c/IR.h>

/******************************************************************************/
/*  SOLUTION GRAPH                                                            */
/******************************************************************************/

extern "C" AIUSolutionCode AIUGetSolutionCode(AIUSolution solution_);

/* -- Generated kernels and call graph */
extern "C" MlirModule AIUGetModule(AIUSolution solution_);

/*   - EGraph */
/*   - Binary(s) */
extern "C" AIUResultCode AIUGetBinary(AIUSolution solution_,
                                      const char *kernel_name_,
                                      AIUBinary *result_);

extern "C" AIUResultCode AIUGetObject(AIUBinary bin_, const char **buffer_);

extern "C" AIUResultCode AIUGetLaunchDims(AIUBinary bin_, size_t *size_,
                                          const size_t **dims_);

#endif /* AIUNITE_CLIENT_SOLUTION_H */
