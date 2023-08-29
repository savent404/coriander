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
#include <semaphore.h>

namespace coriander {
namespace os {
namespace posix {

struct Semaphore : public ISemaphore {
  Semaphore() noexcept {
    sem_init(&mSem, 0, 0);
  }
  void wait() noexcept override {
    sem_wait(&mSem);
  }
  bool tryWait() noexcept override {
    return sem_trywait(&mSem) == 0;
  }
  void post() noexcept override {
    sem_post(&mSem);
  }

 private:
  sem_t mSem;
};

}  // namespace posix
}  // namespace os
}  // namespace coriander
