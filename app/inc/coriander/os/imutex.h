/**
 * @file imutex.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-07-27
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once

namespace coriander {
namespace os {
struct IMutex {
  virtual ~IMutex() = default;

  virtual void lock() noexcept = 0;
  virtual void unlock() noexcept = 0;
};
}  // namespace os
}  // namespace coriander
