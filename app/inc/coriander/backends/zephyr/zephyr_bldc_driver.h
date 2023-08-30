/**
 * @file zephyr_bldc_driver.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-31
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once

#include "coriander/motorctl/ibldc_driver.h"

namespace coriander {
namespace motorctl {
namespace zephyr {
struct BldcDriver : public IBldcDriver {
  virtual ~BldcDriver() = default;
  virtual void enable();
  virtual void disable();
  virtual void emergencyBreak();
  /**
   * @brief Set the phase duty cycles
   *
   * @param u duty cycle of phase U
   * @param v duty cycle of phase V
   * @param w duty cycle of phase W
   */
  virtual void setPhaseDutyCycle(uint16_t u, uint16_t v, uint16_t w);
};
}  // namespace zephyr
}  // namespace motorctl
}  // namespace coriander
