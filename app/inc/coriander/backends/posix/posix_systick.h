/**
 * @file posix_systick.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-03
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once

#include <time.h>

#include "coriander/os/isystick.h"

namespace coriander {
namespace os {
namespace posix {
struct Systick : public ISystick {
  virtual uint32_t systick_us() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
  }
  virtual uint32_t systick_ms() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
  }
};
}  // namespace posix
}  // namespace os
}  // namespace coriander
