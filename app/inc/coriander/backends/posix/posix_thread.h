/**
 * @file posix_thread.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-10
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once
#include <pthread.h>

#include "coriander/os/ithread.h"

namespace coriander {
namespace os {
namespace posix {

struct Thread : public coriander::os::IThread {
  virtual size_t currentThreadId() {
    return reinterpret_cast<size_t>(pthread_self());
  }
};

}  // namespace posix
}  // namespace os
}  // namespace coriander
