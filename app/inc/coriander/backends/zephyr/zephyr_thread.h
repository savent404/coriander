/**
 * @file zephyr_thread.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-10
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once

#include <zephyr/kernel.h>

#include "coriander/os/ithread.h"

namespace coriander {
namespace os {
namespace zephyr {
class Thread : public IThread {
 public:
  virtual size_t currentThreadId() override {
    return reinterpret_cast<size_t>(k_current_get());
  }
};
}  // namespace zephyr
}  // namespace os
}  // namespace coriander