/**
 * @file isemaphore.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-10
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once

namespace coriander {
namespace os {
struct ISemaphore {
  virtual ~ISemaphore() = default;
  virtual void wait() noexcept = 0;
  virtual bool tryWait() noexcept = 0;
  virtual void post() noexcept = 0;
};
}  // namespace os
}  // namespace coriander
