/**
 * @file ilogger.h
 * @author Savent Gate(savent_gate@outlook.com)
 * @brief
 * @date 2023-07-31
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once
#include <fmt/format.h>

#include <memory>

namespace coriander {
namespace base {

enum class LogLevel : unsigned char {
  L_Trace = 0,
  L_Debug,
  L_Info,
  L_Warn,
  L_Error,
  L_MAXIUM,
};

#ifndef CONFIG_CORIANDER_LOG_LEVEL
#define CONFIG_CORIANDER_LOG_LEVEL (2)
#endif

struct ILogger {
  virtual ~ILogger() noexcept = default;
  virtual void log(const char* msg) noexcept = 0;
};

struct LoggerPrefix {
  template <typename OutIter>
  static auto appendPrefix(OutIter out, LogLevel lvl) {
    switch (lvl) {
      case LogLevel::L_Trace:
        return fmt::format_to(out, "[TRACE] ");
      case LogLevel::L_Debug:
        return fmt::format_to(out, "[DEBUG] ");
      case LogLevel::L_Info:
        return fmt::format_to(out, "[INFO] ");
      case LogLevel::L_Warn:
        return fmt::format_to(out, "[WARN] ");
      case LogLevel::L_Error:
        return fmt::format_to(out, "[ERROR] ");
      default:
        return fmt::format_to(out, "[UNKNOWN] ");
    }
  }
};

#define CORIANDER_LOG(logger, level, ...)                              \
  do {                                                                 \
    using buffer = fmt::basic_memory_buffer<char, 64>;                 \
    buffer buf;                                                        \
    auto out = std::back_inserter(buf);                                \
    auto o1 = coriander::base::LoggerPrefix::appendPrefix(out, level); \
    auto o2 = fmt::format_to(o1, __VA_ARGS__);                         \
    *o2++ = '\0';                                                      \
    logger->log(buf.data());                                           \
  } while (0)

#define CORIANDER_LOG_TRACE(logger, ...) \
  CORIANDER_LOG(logger, coriander::base::LogLevel::L_Trace, __VA_ARGS__)
#define CORIANDER_LOG_DEBUG(logger, ...) \
  CORIANDER_LOG(logger, coriander::base::LogLevel::L_Debug, __VA_ARGS__)
#define CORIANDER_LOG_INFO(logger, ...) \
  CORIANDER_LOG(logger, coriander::base::LogLevel::L_Info, __VA_ARGS__)
#define CORIANDER_LOG_WARN(logger, ...) \
  CORIANDER_LOG(logger, coriander::base::LogLevel::L_Warn, __VA_ARGS__)
#define CORIANDER_LOG_ERROR(logger, ...) \
  CORIANDER_LOG(logger, coriander::base::LogLevel::L_Error, __VA_ARGS__)

}  // namespace base
}  // namespace coriander
