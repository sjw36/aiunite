/*! \file
 *
 *   \brief AIUnite Service API
 *   \author Simon Waters
 *
 *   Kernel Generator API to service Execution Engines.
 *
 *   \version 0.1.0
 *   \date 04-12-2023
 *
 *   Doxygen documentation for this header is available here:
 *
 *       https://github.com/ROCmSoftwarePlatform/AIUnite
 *
 *   The latest version of this header can be found on the GitHub releases page:
 *
 *       https://github.com/ROCmSoftwarePlatform/AIUnite/release
 *
 *   Bugs and patches can be submitted to the GitHub repository:
 *
 *       https://github.com/ROCmSoftwarePlatform/AIUnite
 */

#ifndef AIUNITE_SERVICE_H
#define AIUNITE_SERVICE_H

#include <aiunite/common/protocol.h>
#include <aiunite/common/types.h>

#include <aiunite/service/server.h>

#include <mlir-c/IR.h>

/******************************************************************************/
/*  WALK                                                               */
/******************************************************************************/

extern "C" const char *AIUGetMD5(AIURequest request_);

extern "C" MlirModule AIUGetModule(AIURequest request_);

extern "C" AIURequestCode AIUGetRequestCode(AIURequest request_);

/******************************************************************************/
/*  SOLUTION RESPONSE                                                         */
/******************************************************************************/

extern "C" AIUResultCode AIUSendModule(AIUSolution solution_,
                                       MlirModule module_);

extern "C" AIUResultCode AIUSendErrorCode(AIUSolution solution_,
                                          AIUResponseCode code_);

#endif /* AIUNITE_SERVICE_H */
