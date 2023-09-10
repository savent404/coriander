/**
 * @file motor_ctl_openloop.cc
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-09-02
 *
 * Copyright 2023 savent_gate
 *
 */
#include "coriander/motorctl/motor_ctl_openloop.h"

#include <math.h>

#include "coriander/motorctl/foc.h"

#ifndef CONFIG_JSCOPE_ENABLE
#define CONFIG_JSCOPE_ENABLE 0
#endif

#if CONFIG_JSCOPE_ENABLE
#define ATTR_JSCOPE \
  __attribute__((section(".data.jscope"))) __attribute__((used))
ATTR_JSCOPE static float _dOpenLoopElecAngle = 0.0f;
ATTR_JSCOPE static float _dOpenLoopSensorElecAngle = 0.0f;
ATTR_JSCOPE static float _dOpenLoopIq = 0.0f;
ATTR_JSCOPE static float _dOpenLoopId = 0.0f;
ATTR_JSCOPE static float _dOpenLoopVel = 0.0f;
#endif

namespace coriander::motorctl {

void MotorCtlOpenLoop::start() {
  float supplyVoltage =
      mParameters->getValue<float>(ParamId::MotorCtl_MotorDriver_SupplyVoltage);
  float outVoltage =
      mParameters->getValue<float>(ParamId::MotorCtl_OpenLoop_OutVoltage);
  mUseElecAngle =
      mParameters->getValue<int32_t>(ParamId::MotorCtl_OpenLoop_UseElecAngle);
  mDutyCycleUq = outVoltage / supplyVoltage;
  mDurationTimeout->setDuration(1);
  mDurationTimeout->reset();
  mSensorHandler.enable();
  mFocMotorDriver->enable();
}

void MotorCtlOpenLoop::stop() {
  mFocMotorDriver->disable();
  mSensorHandler.disable();
}

void MotorCtlOpenLoop::loop() {
  if (mDurationTimeout->expired()) {
    mSensorHandler.sync();
    mDurationTimeout->reset();
    mFocMotorDriver->setVoltageNoSensor(0, mDutyCycleUq, mCurrentAngle);
#if CONFIG_JSCOPE_ENABLE
    _dOpenLoopElecAngle = mCurrentAngle;
    _dOpenLoopSensorElecAngle = mElecAngleEstimator->getElectricalAngle();
    float alpha, beta, sineTheta, cosineTheta, theta, d, q;
    mPhaseCurrentEstimator->getPhaseCurrent(&alpha, &beta);
    theta = _dOpenLoopSensorElecAngle / 180.0f * 3.1415926f;
    sineTheta = sinf(theta);
    cosineTheta = cosf(theta);
    foc::park(alpha, beta, sineTheta, cosineTheta, &d, &q);
    _dOpenLoopId = d * 1000;
    _dOpenLoopIq = q * 1000;
    _dOpenLoopVel = mVelocityEstimator->getVelocity();
#endif

    if (mUseElecAngle) {
      // update elec angle based on sensor
      mCurrentAngle = mElecAngleEstimator->getElectricalAngle();
    } else {
      // fake elec angle
      mCurrentAngle += 10.0f;
      if (mCurrentAngle > 360.0f) {
        mCurrentAngle -= 360.0f;
      }
    }
  }
}

void MotorCtlOpenLoop::emergencyStop() { stop(); }

bool MotorCtlOpenLoop::fatalError() { return false; }

}  // namespace coriander::motorctl
