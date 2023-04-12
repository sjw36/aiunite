/******************************************************************************/
/*  CPX:   AI ComPleX API                                                     */
/*  AI Common Plugin eXchange                                                 */
/******************************************************************************/

#ifndef AIUNITE_SERVICE_SERVER_H
#define AIUNITE_SERVICE_SERVER_H

#include <aiunite/common/protocol.h>
#include <aiunite/common/types.h>

/******************************************************************************/
/*  SERVICE MGMT                                                              */
/******************************************************************************/

typedef AIUResponseCode (*AIUCallBack)(AIURequest request,
                                       AIUSolution solution);

extern "C" AIUResultCode AIUCreateService(int port_, AIUCallBack callback_);

extern "C" AIUResultCode AIUShutdownService();

#endif /* AIUNITE_SERVICE_SERVER_H */
