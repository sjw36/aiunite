/******************************************************************************/
/*  AIU:   OPEN Kernel Generator Interface                                   */
/*  AIKGI:  AI Kernel Generator Interface                                     */
/*  AIAPI:  AI Accelerator (Programming) Interface                            */
/******************************************************************************/

#include <aiunite/client.h>
#include <_aiu/client/devices.h>
#include <_aiu/support.h>
#include <_aiu/logger.h>

extern "C" AIUResultCode AIUInitialize() {
  AIU_LOG_FUNC(AIUInitialize);

  // static: find devices in system
  AIUDevices::get();

  return AIU_SUCCESS;
}

extern "C" AIUResultCode AIUGetDevice(int64_t index, AIUDevice *result) {
  AIU_LOG_FUNC(AIUGetDevice);
  AIU_CHECK_RESULT(result);

  *result = AIUDevices::get().get(index);
  return AIU_SUCCESS;
}

extern "C" AIUResultCode AIUGetDeviceByType(AIUDeviceType type,
                                            AIUDevice *result) {
  AIU_LOG_FUNC(AIUGetDeviceByType);
  AIU_CHECK_RESULT(result);

  *result = AIUDevices::get().lookup(type);
  return AIU_SUCCESS;
}

extern "C" AIUResultCode AIUGetDeviceInfo(AIUDevice device, AIUDeviceInfo prop,
                                          int64_t prop_value_size,
                                          void *prop_value,
                                          int64_t *prop_value_size_ret) {
  AIU_LOG_FUNC(AIUGetDeviceInfo);
  return AIU_SUCCESS;
}
