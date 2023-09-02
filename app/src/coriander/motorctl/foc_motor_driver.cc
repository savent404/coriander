/**
 * @file ifoc_motor_driver.cc
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-05
 *
 * Copyright 2023 savent_gate
 *
 */
#include "coriander/motorctl/foc_motor_driver.h"

#include "coriander/base/math.h"
#include "coriander/motorctl/foc.h"

namespace coriander {
namespace motorctl {

void FocMotorDriverBase::setVoltageNoSensor(float d, float q, float angle) {
  float elecAngle = angle;
  float elecAngleRad = elecAngle / 180.0f * M_PI;
  float sinTheta = sinf(elecAngleRad);
  float cosTheta = cosf(elecAngleRad);
  float alpha, beta;
  float vu, vv, vw;
  uint16_t u, v, w;

  foc::invPark(d, q, sinTheta, cosTheta, &alpha, &beta);
  foc::SpaceVectorPwm(alpha, beta, &vu, &vv, &vw);
  u = static_cast<uint16_t>(vu * UINT16_MAX);
  v = static_cast<uint16_t>(vv * UINT16_MAX);
  w = static_cast<uint16_t>(vw * UINT16_MAX);
  setPhaseDutyCycle(u, v, w);
}

void FocMotorDriver::setVoltage(float d, float q) {
  float angle = mElecAngleEstimator->getElectricalAngle();
  setVoltageNoSensor(d, q, angle);
}

}  // namespace motorctl
}  // namespace coriander
