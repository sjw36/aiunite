/******************************************************************************/
/*  CPX:   AI ComPleX API                                                     */
/*  Common Pluggable eXchange                                                 */
/******************************************************************************/

#ifndef AIUNITE_COMMS_H
#define AIUNITE_COMMS_H

#include <aiunite/types.h>

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

extern "C" AIUResultCode AIURecvSolution(AIURequest request_,
                                         AIUSolution *result_);

extern "C" AIUResultCode AIURecvSolutionFromDevice(AIURequest request_,
                                                   AIUDevice device_,
                                                   AIUSolution *result_);

#endif /* AIUNITE_COMMS_H */