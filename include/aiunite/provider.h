/******************************************************************************/
/*  CPX:   AI ComPleX API                                                     */
/*  AI Common Plugin eXchange                                                 */
/******************************************************************************/

#ifndef AIUNITE_SERVER_H
#define AIUNITE_SERVER_H

#include "protocol.h"

#include <mlir-c/IR.h>

/******************************************************************************/
/*  REGISTRY MGMT                                                             */
/******************************************************************************/

extern "C"
AIUResult
AIURegister(const char *filename);

/******************************************************************************/
/*  SERVICE MGMT                                                              */
/******************************************************************************/

struct _AIUContext;
typedef _AIUContext *AIUContext;

struct _AIURequest;
typedef _AIURequest *AIURequest;

typedef void (*AIUCallBack)(AIURequest request);

extern "C"
AIUResult
AIUCreateService(int port, AIUCallBack cb);


/******************************************************************************/
/*  SERVICE MGMT                                                              */
/******************************************************************************/



#endif /* AIUNITE_SERVER_H */
