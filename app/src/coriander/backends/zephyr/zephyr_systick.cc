/**
 * @file zephyr_systick.cc
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-07
 *
 * Copyright 2023 savent_gate
 *
 */
#include "zephyr/zephyr_systick.h"

#include <zephyr/kernel.h>
#include <zephyr/sys_clock.h>

namespace coriander {
namespace os {
namespace zephyr {

uint32_t Systick::systick_ms() {
  auto tick = sys_clock_tick_get();
  return static_cast<uint32_t>(
      k_ticks_to_ms_floor64(static_cast<uint64_t>(tick)));
}
uint32_t Systick::systick_us() {
  auto tick = sys_clock_tick_get();
  return static_cast<uint32_t>(
      k_ticks_to_us_floor64(static_cast<uint64_t>(tick)));
}

}  // namespace zephyr
}  // namespace os
}  // namespace coriander
