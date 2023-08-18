/**
 * @file foc.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-03
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

#include <cstdint>

namespace coriander {
namespace motorctl {
namespace foc {
static inline void clarke(float a, float b, float c, float *alpha,
                          float *beta) {
  *alpha = a;
  *beta = (0.57735026919f * a + 1.15470053838f * b);
}

static inline void invClarke(float alpha, float beta, float *a, float *b) {
  *a = alpha;
  *b = -0.5f * alpha + 0.86602540378f * beta;
}

static inline void park(float alpha, float beta, float sinTheta, float cosTheta,
                        float *d, float *q) {
  *d = alpha * cosTheta + beta * sinTheta;
  *q = -alpha * sinTheta + beta * cosTheta;
}

static inline void invPark(float d, float q, float sinTheta, float cosTheta,
                           float *alpha, float *beta) {
  *alpha = d * cosTheta - q * sinTheta;
  *beta = d * sinTheta + q * cosTheta;
}

/**
 * @brief Space Vector Pulse Width Modulation
 *
 * @param alpha duty cycle of axis x
 * @param beta  duty cycle of axis y
 * @param du    duty cycle of phase u
 * @param dv    duty cycle of phase v
 * @param dw    duty cycle of phase w
 */
void SpaceVectorPwm(float alpha, float beta, float *du, float *dv, float *dw);

};  // namespace foc
}  // namespace motorctl
}  // namespace coriander
