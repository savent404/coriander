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

namespace coriander {
namespace base {

struct ILogger {
  virtual ~ILogger() noexcept = default;
  virtual void log(const char* msg) noexcept = 0;
};

}  // namespace base
}  // namespace coriander
