/**
 * @file motor_ctl_velocity.cc
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-06
 *
 * Copyright 2023 savent_gate
 *
 */
#include "coriander/motorctl/motor_ctl_velocity.h"

namespace coriander {
namespace motorctl {

void MotorCtlVelocity::start() {
  // reset pid
  mVelocityPid.P =
      mParameters->getValue<float>(ParamId::MotorCtl_SpeedCtl_PidP);
  mVelocityPid.I =
      mParameters->getValue<float>(ParamId::MotorCtl_SpeedCtl_PidI);
  mVelocityPid.D =
      mParameters->getValue<float>(ParamId::MotorCtl_SpeedCtl_PidD);
  mVelocityPid.output_ramp =
      mParameters->getValue<float>(ParamId::MotorCtl_SpeedCtl_PidOutputRamp);
  mVelocityPid.limit =
      mParameters->getValue<float>(ParamId::MotorCtl_SpeedCtl_PidLimit);
  mVelocityPid.reset();

  // read parameters
  mTargetVelocity =
      mParameters->getValue<float>(ParamId::MotorCtl_General_TargetVelocity_RT);
  mMotorSupplyVoltage =
      mParameters->getValue<float>(ParamId::MotorCtl_MotorDriver_SupplyVoltage);

  mDurationTimeout->setDuration(static_cast<uint32_t>(
      1e6 / mParameters->getValue<uint32_t>(ParamId::MotorCtl_SpeedCtl_Freq)));

  // enable sensors, motor
  mSensorHandler.enable();
  mFocMotorDriver->enable();
}

void MotorCtlVelocity::stop() {
  mSensorHandler.disable();
  mFocMotorDriver->disable();
}

void MotorCtlVelocity::loop() {
  const uint32_t maxDurationUs = 20'000;
  uint32_t durationUs;
  float velocityError;
  float torqueTargetIq, torqueTargetId;
  float torqueTargetUq, torqueTargetUd;

  mSensorHandler.sync();

  // limit calculation frequency
  if (mDurationTimeout->expired()) {
    mDurationTimeout->reset();

    mDurationEstimator->recordStop();

    // limit durationMs
    durationUs = mDurationEstimator->getDuration();
    if (durationUs > maxDurationUs) {
      durationUs = maxDurationUs;
    }

    velocityError = mTargetVelocity - mVelocityEstimator->getVelocity();
    torqueTargetIq = mVelocityPid(velocityError, durationUs * 1.0e-6f);
    torqueTargetId = 0.0f;

    // TODO(savent): add a current feed back closed loop
    torqueTargetUq = torqueTargetIq;
    torqueTargetUd = torqueTargetId;
    mFocMotorDriver->setVoltage(torqueTargetUd, torqueTargetUq);

    mDurationEstimator->recordStart();
  }
}

void MotorCtlVelocity::emergencyStop() {
  this->stop();
  mLogger->log("MotorCtlVelocity emergencyStop");
}

bool MotorCtlVelocity::fatalError() { return false; }

void MotorCtlVelocity::setTargetVelocity(float velocityInRpm) {
  mTargetVelocity = velocityInRpm;
}

}  // namespace motorctl
}  // namespace coriander
