/**
 * @file low_pass_filter.cc
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-06
 *
 * Copyright 2023 savent_gate
 *
 */
#include "coriander/motorctl/low_pass_filter.h"

namespace coriander {
namespace motorctl {

LowPassFilter::LowPassFilter(float time_constant)
    : Tf(time_constant), y_prev(0.0f) {}

float LowPassFilter::operator()(float x, float ts) {
  float dt = ts;
  float alpha = Tf / (Tf + dt);
  float y = alpha * y_prev + (1.0f - alpha) * x;
  y_prev = y;
  return y;
}
}  // namespace motorctl
}  // namespace coriander
