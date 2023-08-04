/**
 * @file ivelocity_estimator.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-08-03
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

#include "coriander/motorctl/isensor.h"

namespace coriander {
namespace motorctl {
  
struct IVelocityEstimator : public ISensor {
  virtual ~IVelocityEstimator() = default;

  /**
   * @brief Get the Velocity object
   * 
   * @return float RPM(Revolutions Per Minute)
   */
  virtual float getVelocity() = 0;
};

}  // namespace motorctl
}  // namespace coriander