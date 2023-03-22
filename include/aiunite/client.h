/******************************************************************************/
/*  CPX:   AI ComPleX API                                                     */
/*  Common Pluggable eXchange                                                 */
/******************************************************************************/

#ifndef AIUNITE_CLIENT_H
#define AIUNITE_CLIENT_H

#include "protocol.h"

#include <mlir-c/IR.h>

/******************************************************************************/
/*  REGISTRY MGMT                                                             */
/******************************************************************************/

extern "C"
AIUResult
AIUReadRegistry(const char *filename);

/******************************************************************************/
/*  CONTEXT MGMT                                                              */
/******************************************************************************/

struct _AIUContext;
typedef _AIUContext *AIUContext;
struct _AIUFunc;
typedef _AIUFunc *AIUFunc;

extern "C"
AIUResult
AIUCreateContext(AIUContext *result);

extern "C"
AIUResult
AIUDestroyContext(AIUContext);

/******************************************************************************/
/*  PROBLEM SPEC                                                              */
/******************************************************************************/

extern "C"
AIUResult
AIUCreateFunc(AIUContext, const char *name, AIUFunc *result);

extern "C"
AIUResult
AIUDestroyFunc(AIUFunc);

/* 1. Generator Spec */
extern "C"
AIUResult
AIUGenerateKernel(AIUContext, const char *options, AIUFunc *result);
/* 
extern "C" AIUResult AIUGenerateKernel(AIUContext, AIUOpType optype, ..., AIUFunc *result);
*/

/* 2. Builder Spec */
struct _AIUType;
typedef _AIUType *AIUType;
struct _AIUValue;
typedef _AIUValue *AIUValue;
struct _AIUOperation;
typedef _AIUOperation *AIUOperation;

/*  - types */
extern "C"
AIUResult
AIUMakeTensorType(AIUContext, int dims[], AIUType elemType, AIUType *result);

/*  - kernel func */
extern "C"
AIUResult
AIUAddFunctionParam(AIUContext, AIUFunc func, AIUType type, AIUValue *result);


/* 3. Clone Spec */
extern "C"
AIUResult
AIUCloneFunc(AIUContext, MlirOperation kernel, AIUFunc *result);

/******************************************************************************/
/*  SUBMIT REQUEST, RECEIVE SOLUTION                                          */
/******************************************************************************/

struct _AIURequest;
typedef _AIURequest *AIURequest;

struct _AIUSolution;
typedef _AIUSolution *AIUSolution;

extern "C"
AIUResult
AIUSubmitFunc(AIUContext, AIUFunc kernel, AIUAction action, AIURequest *result);

// with provider?

extern "C"
AIUResult
AIUReceiveSolution(AIUContext, AIUFunc kernel, AIUSolution *result);

/******************************************************************************/
/*  SOLUTION SPEC                                                             */
/******************************************************************************/

struct _AIUBinary;
typedef _AIUBinary *AIUBinary;

/* -- Generated kernels and call graph */
extern "C"
AIUResult
AIUGetObject(AIUSolution solution, AIUFunc kernel, AIUBinary *result);
/*   - EGraph */
/*   - Binary(s) */

/* -- Tuning space */

#endif /* AIUNITE_CLIENT_H */

