/**
 * @file zephyr_semaphore.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-10
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once

#include <zephyr/kernel.h>
#include <zephyr/sys/sem.h>

#include "coriander/os/isemaphore.h"

namespace coriander {
namespace os {
namespace zephyr {

struct Semaphore : public ISemaphore {
  explicit Semaphore() noexcept { k_sem_init(&mSemaphore, 0, 3); }
  virtual void wait() noexcept override { k_sem_take(&mSemaphore, K_FOREVER); }
  virtual bool tryWait() noexcept override {
    return k_sem_take(&mSemaphore, K_NO_WAIT) == 0;
  }
  virtual void post() noexcept override { k_sem_give(&mSemaphore); }

 private:
  struct k_sem mSemaphore;
};

}  // namespace zephyr
}  // namespace os
}  // namespace coriander
