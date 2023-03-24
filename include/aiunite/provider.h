/******************************************************************************/
/*  CPX:   AI ComPleX API                                                     */
/*  AI Common Plugin eXchange                                                 */
/******************************************************************************/

#ifndef AIUNITE_SERVER_H
#define AIUNITE_SERVER_H

#include <aiunite/protocol.h>

#include <mlir-c/IR.h>

/******************************************************************************/
/*  REGISTRY MGMT                                                             */
/******************************************************************************/

extern "C" AIUResultCode AIURegister(const char *filename);

/******************************************************************************/
/*  SERVICE MGMT                                                              */
/******************************************************************************/

struct _AIUContext;
typedef _AIUContext *AIUContext;

struct _AIURequest;
typedef _AIURequest *AIURequest;

struct _AIUSolution;
typedef _AIUSolution *AIUSolution;

typedef AIUResponseCode (*AIUCallBack)(AIURequest request,
                                       AIUSolution solution);

extern "C" AIUResultCode AIUCreateService(int port, AIUCallBack cb);

/******************************************************************************/
/*  WALK                                                               */
/******************************************************************************/

extern "C" MlirModule AIUGetModule(AIURequest request);

extern "C" AIURequestCode AIUGetRequestCode(AIURequest request);

extern "C" AIUResultCode AIUSetModule(AIUSolution solution, MlirModule module);

#endif /* AIUNITE_SERVER_H */
