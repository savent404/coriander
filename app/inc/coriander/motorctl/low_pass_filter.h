/**
 * @file low_pass_filter.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-06
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once

namespace coriander {
namespace motorctl {

struct LowPassFilter {
 public:
  /**
   * @param Tf - Low pass filter time constant
   */
  LowPassFilter(float Tf);
  ~LowPassFilter() = default;

  float operator()(float x, float Ts);
  float Tf;  //!< Low pass filter time constant

 protected:
  float y_prev;  //!< filtered value in previous execution step
};

}  // namespace motorctl
}  // namespace coriander
