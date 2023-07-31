/**
 * @file posix_logger.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-07-31
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once

#include <coriander/base/ilogger.h>

#include <cstdio>

namespace coriander {
namespace base {
namespace posix {
struct Logger : public ILogger {
  virtual void log(const char *msg) noexcept override {
    std::printf("%s\n", msg);
  }
};
}  // namespace posix
}  // namespace base
}  // namespace coriander
