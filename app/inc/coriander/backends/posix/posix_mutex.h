/**
 * @file posix_mutex.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-12
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once
#include <pthread.h>

#include "coriander/os/imutex.h"

namespace coriander {
namespace os {
namespace posix {
struct Mutex : public IMutex {
  Mutex() { pthread_mutex_init(&mMutex, nullptr); }
  virtual void lock() noexcept { pthread_mutex_lock(&mMutex); }
  virtual void unlock() noexcept { pthread_mutex_unlock(&mMutex); }

 private:
  pthread_mutex_t mMutex;
};
}  // namespace posix
}  // namespace os
}  // namespace coriander
