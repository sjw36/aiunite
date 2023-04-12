
/*! \file
 *
 *   \brief AIUnite API
 *   \author Simon Waters
 *
 *   Kernel Generator Plugin API for Execution Engines.
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

#ifndef AIUNITE_COMMON_TYPES_H
#define AIUNITE_COMMON_TYPES_H

#define AIU_DECLARE_API_TYPE(X)                                                    \
  struct _AIU##X;                                                              \
  typedef _AIU##X *AIU##X

/******************************************************************************/
/*  AIUNITE TYPES                                                             */
/******************************************************************************/

/* Device types */
AIU_DECLARE_API_TYPE(Device);
AIU_DECLARE_API_TYPE(DeviceRequest);

/* Build types */
AIU_DECLARE_API_TYPE(Model);
AIU_DECLARE_API_TYPE(Type);
AIU_DECLARE_API_TYPE(Value);
AIU_DECLARE_API_TYPE(Attr);
AIU_DECLARE_API_TYPE(Operation);

/* Communication types */
AIU_DECLARE_API_TYPE(Request);

/* Solution types */
AIU_DECLARE_API_TYPE(Solution);
AIU_DECLARE_API_TYPE(Binary);

#endif /* AIUNITE_COMMON_TYPES_H */
