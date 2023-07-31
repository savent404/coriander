/**
 * @file zephyr_logger.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-07-31
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once

#include <coriander/base/ilogger.h>

namespace coriander {
namespace base {
namespace zephyr {

class Logger : public ILogger {
 public:
  Logger() = default;

  ~Logger() override = default;

  virtual void log(const char *msg) noexcept;
};

}  // namespace zephyr
}  // namespace base
}  // namespace coriander
