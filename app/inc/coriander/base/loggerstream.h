/**
 * @file loggerstream.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-07-31
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once

#include <memory>
#include <sstream>

#include "coriander/base/ilogger.h"

namespace coriander {
namespace base {

struct LoggerStream {
  LoggerStream(std::shared_ptr<ILogger> logger) noexcept
      : mLogger(std::move(logger)) {}
  virtual ~LoggerStream() noexcept = default;

  template <typename T>
  friend LoggerStream& operator<<(LoggerStream& stream, const T& t) noexcept {
    stream.mLogStream << t;
    return stream;
  }

  // match std::endl to flush
  friend LoggerStream& operator<<(LoggerStream& stream,
                                  std::ostream& (*pf)(std::ostream&)) {
    // pf(stream.mLogStream); // no need to insert newline
    stream.flush();
    return stream;
  }

  void flush() {
    if (mLogger) {
      mLogger->log(mLogStream.str().c_str());
    }
    mLogStream.clear();
    mLogStream.str("");
  }

 private:
  std::stringstream mLogStream;
  std::shared_ptr<ILogger> mLogger;
};
}  // namespace base
}  // namespace coriander
