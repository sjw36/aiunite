/******************************************************************************/
/*  AIU:   OPEN Kernel Generator Interface                                   */
/*  AIKGI:  AI Kernel Generator Interface                                     */
/*  AIAPI:  AI Accelerator (Programming) Interface                            */
/******************************************************************************/

#include <aiunite/internal/logger.h>

#include <boost/log/utility/setup/common_attributes.hpp>

namespace logging = boost::log;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;

AIULog::AIULog() {
  const char *env_var = "AIU_LOG_FILE";
  if (char *logfile = getenv(env_var)) {
    boost::log::v2_mt_posix::add_common_attributes();
    logging::add_file_log(keywords::file_name = logfile,
                          keywords::rotation_size = 10 * 1024 * 1024,
                          keywords::time_based_rotation =
                              sinks::file::rotation_at_time_point(0, 0, 0),
                          keywords::format = "[%TimeStamp%]: %Message%");
  }
}
