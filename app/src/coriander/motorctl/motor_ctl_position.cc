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

#include "coriander/base/jscope.h"

#if CONFIG_JSCOPE_ENABLE
ATTR_JSCOPE static float _dPosCurr = 0.0f;
ATTR_JSCOPE static float _dPosErr = 0.0f;
ATTR_JSCOPE static float _dPosTarget = 0.0f;
#endif

namespace coriander {
namespace motorctl {

void MotorCtlPosition::start() {
  mMechAnglePid.P = mParameters->getValue<float>(ParamId::MotorCtl_PosCtl_PidP);
  mMechAnglePid.I = mParameters->getValue<float>(ParamId::MotorCtl_PosCtl_PidI);
  mMechAnglePid.D = mParameters->getValue<float>(ParamId::MotorCtl_PosCtl_PidD);
  mMechAnglePid.output_ramp =
      mParameters->getValue<float>(ParamId::MotorCtl_PosCtl_PidOutputRamp);
  mMechAnglePid.limit =
      mParameters->getValue<float>(ParamId::MotorCtl_PosCtl_PidLimit);
  mMechAnglePid.reset();

  mTargetPosition =
      mParameters->getValue<float>(ParamId::MotorCtl_General_TargetPosition_RT);
  mDurationTimeout->setDuration(static_cast<int32_t>(
      1e6 / mParameters->getValue<int32_t>(ParamId::MotorCtl_PosCtl_Freq)));

  mMechLpf.Tf =
      mParameters->getValue<float>(ParamId::MotorCtl_PosCtl_Lpf_TimeConstant);
  mMechLpf.clear();

  mDurationEstimator->reset();
  mSensorHandler.enable();

  // next level start
  mMotorCtlVelocity->start();
}

void MotorCtlPosition::stop() {
  mSensorHandler.disable();

  // next level stop
  mMotorCtlVelocity->stop();
}

void MotorCtlPosition::loop() {
  const uint32_t maxDurationUs = 20'000;
  uint32_t durationUs;
  float mechAngle, mechAngleError;
  float targetVelocity;

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

    mechAngle = mMechLpf(mMechAngleEstimator->getMechanicalAngle(),
                         durationUs * 1.0e-6f);
    mechAngleError = mTargetPosition - mechAngle;
    targetVelocity = mMechAnglePid(mechAngleError, durationUs * 1.0e-6f);
    mMotorCtlVelocity->setTargetVelocity(targetVelocity);

    mDurationEstimator->recordStart();

#if CONFIG_JSCOPE_ENABLE
    _dPosCurr = mechAngle;
    _dPosErr = mechAngleError;
    _dPosTarget = mTargetPosition;
#endif
  }
  // next level loop
  mMotorCtlVelocity->loop();
}

void MotorCtlPosition::emergencyStop() { mMotorCtlVelocity->emergencyStop(); }

bool MotorCtlPosition::fatalError() { return mMotorCtlVelocity->fatalError(); }

}  // namespace motorctl
}  // namespace coriander
