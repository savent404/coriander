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

#include <assert.h>
#include <math.h>

#include <cmath>

namespace {

// slow version of svpwm
static inline void svpwm_slow(float alpha, float beta, float Ts, float Udc,
                              float* tu, float* tv, float* tw) {
  float theta, sector_angle, u_ref, u_ref_percent, ts_half, t0_half, t1, t2,
      v[4], o[3];
  int sector;

  constexpr const float sqrt_3 = 1.7320508075688772935274463415059f;
  constexpr const float pi = 3.1415926535897932384626433832795f;
  constexpr const float pi_3 = pi / 3;

  // calculate sector and sector_angle
  theta = atan2f(beta, alpha) - pi;
  if (theta < 0) {
    theta += 2 * pi;
  }

  sector = static_cast<int>(floorf(theta / pi_3));
  sector_angle = theta - sector * pi_3;

  // calulate reference voltage
  u_ref = sqrtf(alpha * alpha + beta * beta);
  if (u_ref > Udc) {
    u_ref = Udc;
  }
  u_ref_percent = u_ref * sqrt_3 / Udc;

  // calulate switch time
  ts_half = Ts / 2;
  t1 = u_ref_percent * sin(pi_3 - sector_angle) * ts_half;
  t2 = u_ref_percent * sin(sector_angle) * ts_half;
  t0_half = (ts_half - t1 - t2) / 2;

  v[0] = t0_half;
  v[1] = t0_half + t1;
  v[2] = t0_half + t2;
  v[3] = t0_half + t1 + t2;

  switch (sector) {
    case 0:
      o[0] = v[0];
      o[1] = v[1];
      o[2] = v[3];
      break;
    case 1:
      o[0] = v[2];
      o[1] = v[0];
      o[2] = v[3];
      break;
    case 2:
      o[0] = v[3];
      o[1] = v[0];
      o[2] = v[1];
      break;
    case 3:
      o[0] = v[3];
      o[1] = v[2];
      o[2] = v[0];
      break;
    case 4:
      o[0] = v[1];
      o[1] = v[3];
      o[2] = v[0];
      break;
    case 5:
      o[0] = v[0];
      o[1] = v[3];
      o[2] = v[2];
      break;
  }

  // centerize output
  float max = o[0], min = o[0], center_offset;
  for (int i = 1; i < 3; i++) {
    if (max < o[i]) {
      max = o[i];
    }
    if (min > o[i]) {
      min = o[i];
    }
  }
  center_offset = ts_half - (max + min) / 2;
  for (int i = 0; i < 3; i++) {
    o[i] += center_offset;
    assert(o[i] > 0 && o[i] < Ts);
  }
  *tu = o[0];
  *tv = o[1];
  *tw = o[2];
}
}  // namespace

namespace coriander {
namespace motorctl {

void foc::SpaceVectorPwm(float alpha, float beta, float* du, float* dv,
                         float* dw) {
  ::svpwm_slow(alpha, beta, 1, 1, du, dv, dw);
}

}  // namespace motorctl
}  // namespace coriander
