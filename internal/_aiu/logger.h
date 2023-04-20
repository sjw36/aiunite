/******************************************************************************/
/*  CPX:   AI Unite API                                                       */
/*  Common Pluggable eXchange                                                 */
/******************************************************************************/

#ifndef _AIU_LOGGER_H
#define _AIU_LOGGER_H

#ifndef BOOST_NO_RTTI

#define BOOST_LOG_DYN_LINK 1
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/file.hpp>

class AIULog : public ::boost::log::sources::severity_logger<
                   ::boost::log::trivial::severity_level> {
  AIULog();

public:
  static AIULog &get() {
    static AIULog s_log;
    return s_log;
  }

  class FuncTrace {
    const char *name;
  public:
    FuncTrace(const char *_n) : name(_n) {
      BOOST_LOG(AIULog::get()) << "API STRT: " << name;
    }
    template <typename... A>
    FuncTrace(const char *_n, A&... a) {
      // collect all args, and print comma-separated
    }
    ~FuncTrace() {
      BOOST_LOG(AIULog::get()) << "API DONE: " << name;
    }
  };
};

#ifdef AIU_LOGGING_ENABLED
#define AIU_LOG_INFO(X) BOOST_LOG_SEV(AIULog::get(), ::boost::log::trivial::info) << X
#define AIU_LOG_DBG(X) BOOST_LOG_SEV(AIULog::get(), ::boost::log::trivial::debug) << X
#define AIU_LOG_ERROR(X) BOOST_LOG_SEV(AIULog::get(), ::boost::log::trivial::error) << X
#define AIU_LOG_FUNC(X) AIULog::FuncTrace _AIU_LOG_FUNC_TRACE(#X)
//#define AIU_LOG1(X,ARGS...) AIULog::FuncTrace _AIU_LOG_FUNC_TRACE(#X, ARGS)
#endif
#endif // BOOST_NO_RTTI

#ifndef AIU_LOG_INFO
#include <iostream>
#define AIU_LOG_INFO(X) std::cout << "LOG(info): " << X << std::endl
#define AIU_LOG_DBG(X) std::cout << "LOG(debug): " << X << std::endl
#define AIU_LOG_ERROR(X) std::cout << "LOG(error): " << X << std::endl
#define AIU_LOG_FUNC(X) std::cout << "API(func): " << #X << std::endl
#endif

#endif /* _AIU_LOGGER_H */
