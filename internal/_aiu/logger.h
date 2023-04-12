/******************************************************************************/
/*  CPX:   AI Unite API                                                       */
/*  Common Pluggable eXchange                                                 */
/******************************************************************************/

#ifndef _AIU_CLIENT_LOGGER_H
#define _AIU_CLIENT_LOGGER_H

#define BOOST_LOG_DYN_LINK 1
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/file.hpp>

#define AIU_LOG(X)                                                             \
  const char *AIU_LOG_FUNC_NAME = #X;                                          \
  BOOST_LOG(AIULog::get()) << AIU_LOG_FUNC_NAME
#define AIU_LOG_INFO BOOST_LOG_SEV(AIULog::get(), ::boost::log::trivial::info)
#define AIU_LOG_DBG BOOST_LOG_SEV(AIULog::get(), ::boost::log::trivial::debug)
#define AIU_LOG_ERROR BOOST_LOG_SEV(AIULog::get(), ::boost::log::trivial::error)

class AIULog : public ::boost::log::sources::severity_logger<
                   ::boost::log::trivial::severity_level> {
  AIULog();

public:
  static AIULog &get() {
    static AIULog s_log;
    return s_log;
  }
};

#endif /* _AIU_CLIENT_LOGGER_H */
