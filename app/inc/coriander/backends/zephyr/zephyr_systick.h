/**
 * @file zephyr_systick.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-07
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once

#include <cstdint>

#include "coriander/os/isystick.h"

namespace coriander {
namespace os {
namespace zephyr {
using os::ISystick;
struct Systick : public ISystick {
  virtual uint32_t systick_us();
  virtual uint32_t systick_ms();
};
}  // namespace zephyr
}  // namespace os
}  // namespace coriander
