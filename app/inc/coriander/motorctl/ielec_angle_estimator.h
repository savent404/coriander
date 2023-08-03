/**
 * @file ielec_angle_estimator.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-03
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

#include <cstdint>

#include "coriander/motorctl/isensor.h"

namespace coriander {
namespace motorctl {

struct IElecAngleEstimator : public ISensor {
  /**
   * @brief Get the Electrical Angle object
   *
   * @return degree
   */
  virtual float getElectricalAngle() noexcept = 0;
};

}  // namespace motorctl
}  // namespace coriander
