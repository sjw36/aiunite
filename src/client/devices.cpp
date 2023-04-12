/******************************************************************************/
/*  AIU:   OPEN Kernel Generator Interface                                   */
/*  AIKGI:  AI Kernel Generator Interface                                     */
/*  AIAPI:  AI Accelerator (Programming) Interface                            */
/******************************************************************************/

#include <aiunite/client.h>
#include <aiunite/devices.h>
#include <aiunite/internal/devices.h>
#include <aiunite/internal/support.h>

#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/file.hpp>

namespace logging = boost::log;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;

extern "C" AIUResultCode AIUInitialize() {

#if 0
  logging::add_file_log(
        keywords::file_name = "sample_%N.log",
        keywords::rotation_size = 10 * 1024 * 1024,
        keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0),
        keywords::format = "[%TimeStamp%]: %Message%"
    );
#endif
  // static: find devices in system
  AIUDevices::get();

  return AIU_SUCCESS;
}

extern "C" AIUResultCode AIUGetDevice(int64_t index, AIUDevice *result) {
  AIU_CHECK_RESULT(result);

  *result = AIUDevices::get().get(index);
  return AIU_SUCCESS;
}

extern "C" AIUResultCode AIUGetDeviceByType(AIUDeviceType type,
                                            AIUDevice *result) {
  AIU_CHECK_RESULT(result);

  *result = AIUDevices::get().lookup(type);
  return AIU_SUCCESS;
}

extern "C" AIUResultCode AIUGetDeviceInfo(AIUDevice device, AIUDeviceInfo prop,
                                          int64_t prop_value_size,
                                          void *prop_value,
                                          int64_t *prop_value_size_ret);
