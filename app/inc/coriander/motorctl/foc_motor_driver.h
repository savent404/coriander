/**
 * @file foc_motor_driver.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-05
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once

#include <memory>

#include "coriander/motorctl/ibldc_driver.h"
#include "coriander/motorctl/ielec_angle_estimator.h"

namespace coriander {
namespace motorctl {

struct FocMotorDriver : public IBldcDriver {
  FocMotorDriver(std::shared_ptr<IElecAngleEstimator> elecAngleEstimator)
      : mElecAngleEstimator(elecAngleEstimator) {}

  /**
   * @brief Set the Voltage object
   *
   * @param d duty cycle of d-axis, pointing to the north pole of the magnet
   * @param q duty cycle of q-axis, ahead of d-axis by 90 degree
   */
  void setVoltage(float d, float q);

 private:
  std::shared_ptr<IElecAngleEstimator> mElecAngleEstimator;
};

}  // namespace motorctl
}  // namespace coriander
