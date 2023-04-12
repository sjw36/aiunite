/******************************************************************************/
/*  CPX:   AI ComPleX API                                                     */
/*  Common Pluggable eXchange                                                 */
/******************************************************************************/

#ifndef AIUNITE_SOLUTION_H
#define AIUNITE_SOLUTION_H

#include <aiunite/types.h>
#include <mlir-c/IR.h>

/******************************************************************************/
/*  SOLUTION GRAPH                                                            */
/******************************************************************************/

extern "C" AIUResponseCode AIUGetResponseCode(AIUSolution solution_);

/* -- Generated kernels and call graph */
extern "C" MlirModule AIUGetModule(AIUSolution solution_);

/*   - EGraph */
/*   - Binary(s) */
extern "C" AIUResultCode AIUGetBinary(AIUSolution solution_,
                                      const char *kernel_name_,
                                      AIUBinary *result_);

extern "C" AIUResultCode AIUGetObject(AIUBinary bin_, char *buffer_,
                                      size_t *size_);

extern "C" AIUResultCode AIUGetLaunchDims(AIUBinary bin_, size_t *global_size_,
                                          size_t *local_size_);

#endif /* AIUNITE_SOLUTION_H */
