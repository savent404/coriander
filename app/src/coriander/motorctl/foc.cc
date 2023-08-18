/**
 * @file foc.cc
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-03
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "coriander/motorctl/foc.h"

#include <cmath>

namespace {

// NOTE: don't use matrix to simplify the code for toolchain optimizing
static inline void SpaceVectorPwmDetailed(float I, float J, float K, float* S1,
                                          float* S2, float* S3) {
  int F1, F2, F3, N, sector;
  float T1, T2, T0half;

  // sector calculation
  if (I > 0) {
    F1 = 1;
  } else {
    F1 = 0;
  }
  if (J > 0) {
    F2 = 1;
  } else {
    F2 = 0;
  }
  if (K > 0) {
    F3 = 1;
  } else {
    F3 = 0;
  }
  N = F1 + 2 * F2 + 4 * F3;

  // convert N to sector
  switch (N) {
    case 1:
      sector = 6;
      break;
    case 2:
      sector = 2;
      break;
    case 3:
      sector = 1;
      break;
    case 4:
      sector = 4;
      break;
    case 5:
      sector = 5;
      break;
    case 6:
      sector = 3;
      break;
    default:
      sector = 0;
      // __ASSERT_NO_MSG(0);
  }

  // sector condition
  switch (sector) {
    case 1:
      T1 = I;
      T2 = J;
      break;
    case 2:
      T1 = -K;
      T2 = -I;
      break;
    case 3:
      T1 = J;
      T2 = K;
      break;
    case 4:
      T1 = -I;
      T2 = -J;
      break;
    case 5:
      T1 = K;
      T2 = I;
      break;
    case 6:
      T1 = -J;
      T2 = -K;
      break;
    default:
      T1 = T2 = 0;
      // __ASSERT_NO_MSG(0);
      break;
  }
  T0half = .5 * (1 - T1 - T2);

  // duty cycle calculation
  switch (sector) {
    case 1:
      I = T1 + T2 + T0half;
      J = T2 + T0half;
      K = T0half;
      break;
    case 2:
      I = T1 + T0half;
      J = T1 + T2 + T0half;
      K = T0half;
      break;
    case 3:
      I = T0half;
      J = T1 + T2 + T0half;
      K = T2 + T0half;
      break;
    case 4:
      I = T0half;
      J = T1 + T0half;
      K = T1 + T2 + T0half;
      break;
    case 5:
      I = T2 + T0half;
      J = T0half;
      K = T1 + T2 + T0half;
      break;
    case 6:
      I = T1 + T2 + T0half;
      J = T0half;
      K = T1 + T0half;
      break;
    default:
      I = J = K = 0;
      // __ASSERT_NO_MSG(0);
      break;
  }
  *S1 = I;
  *S2 = J;
  *S3 = K;
}
}  // namespace

namespace coriander {
namespace motorctl {

void foc::SpaceVectorPwm(float alpha, float beta, float* du, float* dv,
                         float* dw) {
  constexpr const float sqrt3_2 = 0.86602540378f;
  float i = alpha * sqrt3_2;
  float j = -0.5f * alpha + sqrt3_2 * beta;
  float k = -0.5f * alpha - sqrt3_2 * beta;
  ::SpaceVectorPwmDetailed(i, j, k, du, dv, dw);
}

}  // namespace motorctl
}  // namespace coriander
