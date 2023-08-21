/**
 * @file iphase_current_estimator.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-21
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once

#include "coriander/motorctl/isensor.h"

namespace coriander {
namespace motorctl {

struct IPhaseCurrentEstimator : public ISensor {
  virtual ~IPhaseCurrentEstimator() = default;

  /**
   * @brief Get the Phase Current object
   *
   * @param alpha
   * @param beta
   */
  virtual void getPhaseCurrent(float *alpha, float *beta) = 0;
};

}  // namespace motorctl

}
}