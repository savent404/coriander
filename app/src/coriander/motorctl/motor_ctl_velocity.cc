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

  mVelocityLpf.Tf =
      mParameters->getValue<float>(ParamId::MotorCtl_SpeedCtl_Lpf_TimeConstant);
  mVelocityLpf.clear();

  // enable sensors, motor
  mSensorHandler.enable();

  // next level start
  mMotorCtlCurrent->start();
}

void MotorCtlVelocity::stop() {
  mSensorHandler.disable();

  // next level stop
  mMotorCtlCurrent->stop();
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

    velocityError =
        mTargetVelocity -
        mVelocityLpf(mVelocityEstimator->getVelocity(), durationUs * 1.0e-6f);
    torqueTargetIq = mVelocityPid(velocityError, durationUs * 1.0e-6f);
    torqueTargetId = 0.0f;

    mMotorCtlCurrent->setTargetCurrent(torqueTargetId, torqueTargetIq);

    mDurationEstimator->recordStart();

    // next level loop
    mMotorCtlCurrent->loop();
  }
}

void MotorCtlVelocity::emergencyStop() { this->stop(); }

bool MotorCtlVelocity::fatalError() { return false; }

void MotorCtlVelocity::setTargetVelocity(float velocityInRpm) {
  mTargetVelocity = velocityInRpm;
  mParameters->setValue(ParamId::MotorCtl_General_TargetVelocity_RT,
                        mTargetVelocity);
}

}  // namespace motorctl
}  // namespace coriander
