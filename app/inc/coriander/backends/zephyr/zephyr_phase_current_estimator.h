/**
 * @file zephyr_phase_current_estimator.h
 * @author savent404 (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-30
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once

#include "coriander/motorctl/iphase_current_estimator.h"

namespace coriander {
namespace motorctl {
namespace zephyr {
struct PhaseCurrentEstimator : public IPhaseCurrentEstimator {
  virtual ~PhaseCurrentEstimator() = default;

  virtual void enable();
  virtual void disable();
  virtual bool enabled();
  virtual void sync();
  virtual void getPhaseCurrent(float *alpha, float *beta);
  virtual void calibrate();
  virtual bool needCalibrate();

 private:
  bool calibrateFlag = false;
  float current[3] = {0.0f, 0.0f, 0.0f};
  float offset[3] = {0.0f, 0.0f, 0.0f};
};
}  // namespace zephyr
}  // namespace motorctl
}  // namespace coriander
