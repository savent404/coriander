/**
 * @file math.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-06
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once

#ifndef CONFIG_CMSIS_DSP_FASTMATH
#include <cmath>

#else
#include <arm_math.h>

#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#endif

namespace coriander {
namespace base {
struct math {
  static inline float fmodf(float x, float y) { return std::fmod(x, y); }
};
}  // namespace base
}  // namespace coriander