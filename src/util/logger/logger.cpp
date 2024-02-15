#include "util/logger/logger.h"

namespace logger {

namespace logging = boost::log;
namespace keywords = boost::log::keywords;

void LoggerInit() {
  logging::add_common_attributes();

  logging::core::get()->set_filter(logging::trivial::severity >= logging::trivial::info);

  logging::add_console_log(
      std::clog,
      keywords::format = "[%TimeStamp%]: %Message%",
      keywords::auto_flush = true);
}

void LogInfo(std::string_view tag, std::string_view msg) {
  BOOST_LOG_TRIVIAL(info) << tag << " " << msg;
}

void LogError(std::string_view tag, std::string_view msg) {
  BOOST_LOG_TRIVIAL(error) << tag << " " << msg;
}

void LogFatal(std::string_view msg) {
  BOOST_LOG_TRIVIAL(fatal) << msg;
}

void LogTrace(std::string_view tag, std::string_view msg) {
  BOOST_LOG_TRIVIAL(trace) << tag << " " << msg;
}

}  // namespace logger
