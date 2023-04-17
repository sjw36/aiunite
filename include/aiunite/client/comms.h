/******************************************************************************/
/*  CPX:   AI ComPleX API                                                     */
/*  Common Pluggable eXchange                                                 */
/******************************************************************************/

#ifndef AIUNITE_CLIENT_COMMS_H
#define AIUNITE_CLIENT_COMMS_H

#include <aiunite/common/protocol.h>
#include <aiunite/common/types.h>

/******************************************************************************/
/*  SUBMIT REQUEST, RECEIVE SOLUTION                                          */
/******************************************************************************/

extern "C" AIUResultCode
AIUSendModel(AIUModel model, AIURequestCode request_code, AIURequest *result);

extern "C" AIUResultCode AIUSendModelToDevice(AIUModel model,
                                              AIURequestCode request_code,
                                              AIUDevice device,
                                              AIURequest *result);

/******************************************************************************/
/*  SOLUTION RESPONSE                                                         */
/******************************************************************************/

extern "C" AIUResultCode AIUGetRequestCount(AIURequest request_, int64_t *result_);

extern "C" AIUResultCode AIURecvSolutions(AIURequest request_);

extern "C" AIUResultCode AIUGetSolution(AIURequest request_, size_t index_,
                                         AIUSolution *result_);

extern "C" AIUResultCode AIUGetSolutionFromDevice(AIURequest request_,
                                                  AIUDevice device_,
                                                  AIUSolution *result_);

#endif /* AIUNITE_CLIENT_COMMS_H */
