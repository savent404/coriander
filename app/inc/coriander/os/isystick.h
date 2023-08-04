/**
 * @file isystick.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-03
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once

#include <cstdint>

namespace coriander {
namespace os {
struct ISystick {
  virtual ~ISystick() = default;
  virtual uint32_t systick_us() = 0;
  virtual uint32_t systick_ms() = 0;
};
}  // namespace os
}  // namespace coriander
