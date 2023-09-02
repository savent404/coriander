/**
 * @file zephyr_motor.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-07
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once
#include <cstdint>
#include <memory>

#include "coriander/motorctl/foc_motor_driver.h"
#include "coriander/motorctl/ielec_angle_estimator.h"

namespace coriander {
namespace motorctl {
namespace zephyr {
struct MotorDriver : public coriander::motorctl::FocMotorDriver {
  explicit MotorDriver(std::shared_ptr<IElecAngleEstimator> elecAngleEstimator)
      : coriander::motorctl::FocMotorDriver(elecAngleEstimator) {}
  void enable() override;
  void disable() override;
  void emergencyBreak() override;
  void setPhaseDutyCycle(uint16_t u, uint16_t v, uint16_t w) override;
};
}  // namespace zephyr
}  // namespace motorctl
}  // namespace coriander
