/******************************************************************************/
/*  CPX:   AI ComPleX API                                                     */
/*  Common Pluggable eXchange                                                 */
/******************************************************************************/

#ifndef AIUNITE_CLIENT_H
#define AIUNITE_CLIENT_H

#include <aiunite/protocol.h>

#include <mlir-c/IR.h>

/******************************************************************************/
/*  REGISTRY MGMT                                                             */
/******************************************************************************/

extern "C" AIUResultCode AIUReadRegistry(const char *filename);

/******************************************************************************/
/*  CONTEXT MGMT                                                              */
/******************************************************************************/

struct _AIUContext;
typedef _AIUContext *AIUContext;
struct _AIUModel;
typedef _AIUModel *AIUModel;

extern "C" AIUResultCode AIUCreateContext(AIUContext *result);

extern "C" AIUResultCode AIUDestroyContext(AIUContext);

/******************************************************************************/
/*  PROBLEM SPEC                                                              */
/******************************************************************************/

extern "C" AIUResultCode AIUCreateModel(AIUContext, const char *name,
                                        AIUModel *result);

extern "C" AIUResultCode AIUDestroyModel(AIUModel);

/* 1. Generator Spec */
extern "C" AIUResultCode AIUGenerateModel(AIUContext, const char *options,
                                          AIUModel *result);
/*
extern "C" AIUResultCode AIUGenerateKernel(AIUContext, AIUOpType optype, ...,
AIUModel *result);
*/

/* 2. Builder Spec */
struct _AIUType;
typedef _AIUType *AIUType;
struct _AIUValue;
typedef _AIUValue *AIUValue;
struct _AIUOperation;
typedef _AIUOperation *AIUOperation;

/*  - types */
extern "C" AIUResultCode AIUMakeTensorType(AIUContext, int dims[],
                                           AIUType elemType, AIUType *result);

/*  - kernel func */
extern "C" AIUResultCode AIUAddParameter(AIUContext, AIUModel func,
                                         AIUType type, AIUValue *result);

/* 3. Clone Spec */
extern "C" AIUResultCode AIUCloneModel(AIUContext, MlirOperation kernel,
                                       AIUModel *result);

/******************************************************************************/
/*  SUBMIT REQUEST, RECEIVE SOLUTION                                          */
/******************************************************************************/

struct _AIURequest;
typedef _AIURequest *AIURequest;

struct _AIUSolution;
typedef _AIUSolution *AIUSolution;

extern "C" AIUResultCode
AIUSendModel(AIUModel kernel, AIURequestCode request_code, AIURequest *result);

// with provider?

extern "C" AIUResultCode AIURecvSolution(AIURequest request,
                                         AIUSolution *result);

/******************************************************************************/
/*  SOLUTION SPEC                                                             */
/******************************************************************************/

struct _AIUBinary;
typedef _AIUBinary *AIUBinary;

/* -- Generated kernels and call graph */
extern "C" AIUResultCode AIUGetObject(AIUSolution solution, AIUModel kernel,
                                      AIUBinary *result);
/*   - EGraph */
/*   - Binary(s) */

/* -- Tuning space */

#endif /* AIUNITE_CLIENT_H */

