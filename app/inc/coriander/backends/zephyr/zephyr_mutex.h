/**
 * @file zephyr_mutex.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-12
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once

#include <zephyr/sys/mutex.h>

#include "coriander/os/imutex.h"

namespace coriander {
namespace os {
namespace zephyr {
struct Mutex : public IMutex {
  Mutex() { sys_mutex_init(&mMutex); }
  virtual void lock() noexcept { sys_mutex_lock(&mMutex, K_FOREVER); }
  virtual void unlock() noexcept { sys_mutex_unlock(&mMutex); }

 private:
  struct sys_mutex mMutex;
};
}  // namespace zephyr
}  // namespace os
}  // namespace coriander
