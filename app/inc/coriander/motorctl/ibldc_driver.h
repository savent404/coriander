/**
 * @file ibldc_driver.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-03
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

#include <cstdint>

namespace coriander {
namespace motorctl {
struct IBldcDriver {
  virtual ~IBldcDriver() = default;
  virtual void enable() = 0;
  virtual void disable() = 0;
  virtual void emergencyBreak() = 0;
  /**
   * @brief Set the phase duty cycles
   *
   * @param u duty cycle of phase U
   * @param v duty cycle of phase V
   * @param w duty cycle of phase W
   */
  virtual void setPhaseDutyCycle(uint16_t u, uint16_t v, uint16_t w) = 0;
};
}  // namespace motorctl
}  // namespace coriander
