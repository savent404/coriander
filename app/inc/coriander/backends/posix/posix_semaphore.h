/**
 * @file posix_semaphore.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-10
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once

#include <pthread.h>

#include "coriander/os/isemaphore.h"

namespace coriander {
namespace os {
namespace posix {

struct Semaphore : public ISemaphore {
  explicit Semaphore() noexcept {
    pthread_mutex_init(&mMutex, nullptr);
    pthread_cond_init(&mCond, nullptr);
  }
  virtual void wait() noexcept override {
    pthread_mutex_lock(&mMutex);
    pthread_cond_wait(&mCond, &mMutex);
    pthread_mutex_unlock(&mMutex);
  }
  virtual bool tryWait() noexcept override {
    pthread_mutex_lock(&mMutex);
    int ret = pthread_cond_timedwait(&mCond, &mMutex, nullptr);
    pthread_mutex_unlock(&mMutex);
    return ret == 0;
  }
  virtual void post() noexcept override { pthread_cond_signal(&mCond); }

 private:
  pthread_mutex_t mMutex;
  pthread_cond_t mCond;
};

}  // namespace posix
}  // namespace os
}  // namespace coriander
