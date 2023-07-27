/**
 * @file mutex.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-07-27
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once

#include <zephyr/kernel.h>
#include <zephyr/sys/mutex.h>

#include "coriander/os/imutex.h"

namespace coriander {
namespace os {
namespace zephyr {
struct Mutex : public coriander::IMutex {
  Mutex(const Mutex&) = delete;
  Mutex(Mutex&&) = delete;
  virtual Mutex& operator=(const Mutex&) = delete;

  explicit Mutex() noexcept { sys_mutex_init(&m_mutex); }
  virtual void lock() noexcept override { sys_mutex_lock(&m_mutex, K_FOREVER); }
  virtual void unlock() noexcept override { sys_mutex_unlock(&m_mutex); }

 private:
  struct sys_mutex m_mutex;
}:
}  // namespace zephyr
}  // namespace os
}  // namespace coriander
