/******************************************************************************/
/*  CPX:   AI ComPleX API                                                     */
/*  Common Pluggable eXchange                                                 */
/******************************************************************************/

#ifndef AIUNITE_DEVICES_H
#define AIUNITE_DEVICES_H

#include <aiunite/types.h>

/******************************************************************************/
/*  REGISTRY MGMT                                                             */
/******************************************************************************/

enum AIUDeviceType {
  AIU_DEVICE_TYPE_CPU,
  AIU_DEVICE_TYPE_GPU,
  AIU_DEVICE_TYPE_NPU,
  AIU_DEVICE_TYPE_CUSTOM,
  AIU_DEVICE_TYPE_ALL,
  AIU_DEVICE_TYPE_NONE
};

enum AIUDeviceInfo {
  AIU_DEVICE_TYPE,
  AIU_DEVICE_VENDOR_ID,
  AIU_DEVICE_MAX_COMPUTE_UNITS,
  AIU_DEVICE_MAX_WORK_GROUP_SIZE,
  AIU_DEVICE_ETC
};

extern "C" AIUResultCode AIUInitialize();

extern "C" AIUResultCode AIUGetDevices(int64_t *count, AIUDevice *result);

extern "C" AIUResultCode AIUGetDeviceByType(AIUDeviceType type,
                                            AIUDevice *result);

extern "C" AIUResultCode AIUGetDeviceInfo(AIUDevice device, AIUDeviceInfo prop,
                                          int64_t prop_value_size,
                                          void *prop_value,
                                          int64_t *prop_value_size_ret);

/*
extern "C" AIUResultCode AIUGetDeviceProperties(AIUDevice result,
AIUDeviceProperty);
*/

#endif /* AIUNITE_DEVICES_H */
