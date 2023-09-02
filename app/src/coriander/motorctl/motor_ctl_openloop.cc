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

namespace coriander::motorctl {

void MotorCtlOpenLoop::start() {
  float supplyVoltage =
      mParameters->getValue<float>(ParamId::MotorCtl_MotorDriver_SupplyVoltage);
  float outVoltage =
      mParameters->getValue<float>(ParamId::MotorCtl_OpenLoop_OutVoltage);
  mDutyCycleUd = outVoltage / supplyVoltage;
  mDurationTimeout->setDuration(1);
  mDurationTimeout->reset();
  mFocMotorDriver->enable();
}

void MotorCtlOpenLoop::stop() { mFocMotorDriver->disable(); }

void MotorCtlOpenLoop::loop() {
  if (mDurationTimeout->expired()) {
    mDurationTimeout->reset();
    mFocMotorDriver->setVoltageNoSensor(mDutyCycleUd, 0, mCurrentAngle);
    mCurrentAngle += 0.5f;
    if (mCurrentAngle > 360.0f) {
      mCurrentAngle -= 360.0f;
    }
  }
}

void MotorCtlOpenLoop::emergencyStop() { stop(); }

bool MotorCtlOpenLoop::fatalError() { return false; }

}  // namespace coriander::motorctl
