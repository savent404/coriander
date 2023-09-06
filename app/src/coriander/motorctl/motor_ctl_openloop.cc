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

#ifndef CONFIG_JSCOPE_ENABLE
#define CONFIG_JSCOPE_ENABLE 0
#endif

#if CONFIG_JSCOPE_ENABLE
#define ATTR_JSCOPE \
  __attribute__((section(".data.jscope"))) __attribute__((used))
ATTR_JSCOPE static float _dOpenLoopElecAngle = 0.0f;
#endif

namespace coriander::motorctl {

void MotorCtlOpenLoop::start() {
  float supplyVoltage =
      mParameters->getValue<float>(ParamId::MotorCtl_MotorDriver_SupplyVoltage);
  float outVoltage =
      mParameters->getValue<float>(ParamId::MotorCtl_OpenLoop_OutVoltage);
  mDutyCycleUd = outVoltage / supplyVoltage;
  mDurationTimeout->setDuration(1);
  mDurationTimeout->reset();
  mSensorHandler.enable();
  mFocMotorDriver->enable();
}

void MotorCtlOpenLoop::stop() {
  mFocMotorDriver->disable();
  mSensorHandler.enable();
}

void MotorCtlOpenLoop::loop() {
  if (mDurationTimeout->expired()) {
    mSensorHandler.sync();
    mDurationTimeout->reset();
    mFocMotorDriver->setVoltageNoSensor(mDutyCycleUd, 0, mCurrentAngle);
#if CONFIG_JSCOPE_ENABLE
    _dOpenLoopElecAngle = mCurrentAngle;
#endif
    mCurrentAngle += 10.0f;
    if (mCurrentAngle > 360.0f) {
      mCurrentAngle -= 360.0f;
    }
  }
}

void MotorCtlOpenLoop::emergencyStop() { stop(); }

bool MotorCtlOpenLoop::fatalError() { return false; }

}  // namespace coriander::motorctl
