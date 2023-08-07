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
  mVelocityPid.P = mParameters->getValue<float>("velocity_pid_p"_hash);
  mVelocityPid.I = mParameters->getValue<float>("velocity_pid_i"_hash);
  mVelocityPid.D = mParameters->getValue<float>("velocity_pid_d"_hash);
  mVelocityPid.output_ramp =
      mParameters->getValue<float>("velocity_pid_output_ramp"_hash);
  mVelocityPid.limit = mParameters->getValue<float>("velocity_pid_limit"_hash);
  mVelocityPid.reset();

  // read parameters
  mTargetVelocity = mParameters->getValue<float>("target_velocity"_hash);
  mMotorSupplyVoltage =
      mParameters->getValue<float>("motor_supply_voltage"_hash);

  // enable sensors, motor
  mSensorHandler.enable();
  mFocMotorDriver->enable();
}

void MotorCtlVelocity::stop() {
  mSensorHandler.disable();
  mFocMotorDriver->disable();
}

void MotorCtlVelocity::loop() {
  const uint32_t maxDurationMs = 20;
  uint32_t durationMs;
  float velocityError;
  float torqueTargetIq, torqueTargetId;
  float torqueTargetUq, torqueTargetUd;

  mSensorHandler.sync();
  mDurationEstimator->recordStop();

  // limit durationMs
  durationMs = mDurationEstimator->getDuration();
  if (durationMs > maxDurationMs) {
    durationMs = maxDurationMs;
  }

  velocityError = mTargetVelocity - mVelocityEstimator->getVelocity();
  torqueTargetIq = mVelocityPid(velocityError, durationMs * 1.0e-3f);
  torqueTargetId = 0.0f;

  // TODO: add a current feed back closed loop
  torqueTargetUq = torqueTargetIq;
  torqueTargetUd = torqueTargetId;
  mFocMotorDriver->setVoltage(torqueTargetUd, torqueTargetUq);

  mDurationEstimator->recordStart();
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
