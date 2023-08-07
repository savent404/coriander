/**
 * @file motor_ctl_position.cc
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-08
 *
 * Copyright 2023 savent_gate
 *
 */
#include "coriander/motorctl/motor_ctl_position.h"

namespace coriander {
namespace motorctl {

void MotorCtlPosition::start() {
  mMechAnglePid.P = mParameters->getValue<float>("mech_angle_pid_p"_hash);
  mMechAnglePid.I = mParameters->getValue<float>("mech_angle_pid_i"_hash);
  mMechAnglePid.D = mParameters->getValue<float>("mech_angle_pid_d"_hash);
  mMechAnglePid.output_ramp =
      mParameters->getValue<float>("mech_angle_pid_output_ramp"_hash);
  mMechAnglePid.limit =
      mParameters->getValue<float>("mech_angle_pid_limit"_hash);
  mMechAnglePid.reset();

  mTargetPosition = mParameters->getValue<float>("target_position"_hash);

  mSensorHandler.enable();
  mMotorCtlVelocity->start();
}

void MotorCtlPosition::stop() {
  mSensorHandler.disable();
  mMotorCtlVelocity->stop();
}

void MotorCtlPosition::loop() {
  const uint32_t maxDurationMs = 20;
  uint32_t durationMs;
  float mechAngleError;
  float targetVelocity;

  mSensorHandler.sync();
  mDurationEstimator->recordStop();

  // limit durationMs
  durationMs = mDurationEstimator->getDuration();
  if (durationMs > maxDurationMs) {
    durationMs = maxDurationMs;
  }

  mechAngleError = mTargetPosition - mMechAngleEstimator->getMechanicalAngle();
  targetVelocity = mMechAnglePid(mechAngleError, durationMs * 1.0e-3f);
  mMotorCtlVelocity->setTargetVelocity(targetVelocity);

  // next level loop
  mMotorCtlVelocity->loop();
}

void MotorCtlPosition::emergencyStop() { mMotorCtlVelocity->emergencyStop(); }

bool MotorCtlPosition::fatalError() { return mMotorCtlVelocity->fatalError(); }

}  // namespace motorctl
}  // namespace coriander
