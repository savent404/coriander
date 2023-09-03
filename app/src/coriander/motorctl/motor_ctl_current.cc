/**
 * @file motor_ctl_current.cc
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-21
 *
 * Copyright 2023 savent_gate
 *
 */
#include "coriander/motorctl/motor_ctl_current.h"

namespace coriander {
namespace motorctl {

void MotorCtlCurrent::start() {
  mPidD.P = mParams->getValue<float>(ParamId::MotorCtl_CurrCtl_PidP);
  mPidD.I = mParams->getValue<float>(ParamId::MotorCtl_CurrCtl_PidI);
  mPidD.D = mParams->getValue<float>(ParamId::MotorCtl_CurrCtl_PidD);
  mPidD.output_ramp =
      mParams->getValue<float>(ParamId::MotorCtl_CurrCtl_PidOutputRamp);
  mPidD.limit = mParams->getValue<float>(ParamId::MotorCtl_CurrCtl_PidLimit);
  mPidD.reset();

  mPidQ.P = mParams->getValue<float>(ParamId::MotorCtl_CurrCtl_PidP);
  mPidQ.I = mParams->getValue<float>(ParamId::MotorCtl_CurrCtl_PidI);
  mPidQ.D = mParams->getValue<float>(ParamId::MotorCtl_CurrCtl_PidD);
  mPidQ.output_ramp =
      mParams->getValue<float>(ParamId::MotorCtl_CurrCtl_PidOutputRamp);
  mPidQ.limit = mParams->getValue<float>(ParamId::MotorCtl_CurrCtl_PidLimit);
  mPidQ.reset();

  mDurationTimeout->setDuration(static_cast<uint32_t>(
      1e6 / mParams->getValue<int32_t>(ParamId::MotorCtl_CurrCtl_Freq)));

  mTargetId =
      mParams->getValue<float>(ParamId::MotorCtl_General_TargetCurrentD_RT);
  mTargetIq =
      mParams->getValue<float>(ParamId::MotorCtl_General_TargetCurrentQ_RT);

  mIdLpf.Tf =
      mParams->getValue<float>(ParamId::MotorCtl_CurrCtl_Lpf_TimeConstant);
  mIdLpf.clear();

  mIqLpf.Tf =
      mParams->getValue<float>(ParamId::MotorCtl_CurrCtl_Lpf_TimeConstant);
  mIqLpf.clear();

  // enable sensors, motor
  mPhaseCurrentEstimator->enable();
  mDurationEstimator->recordStart();
  mDurationTimeout->reset();
  mFocMotorDriver->enable();
}

void MotorCtlCurrent::stop() {
  mPhaseCurrentEstimator->disable();
  mFocMotorDriver->disable();
}

void MotorCtlCurrent::loop() {
  const uint32_t maxDurationUs = 1'000;
  uint32_t durationUs;
  float errorId, errorIq;
  float currId, currIq;
  float outputUd, outputUq;

  mPhaseCurrentEstimator->sync();

  // limit calculation frequency
  if (mDurationTimeout->expired()) {
    mDurationTimeout->reset();

    mDurationEstimator->recordStop();

    // limit durationMs
    durationUs = mDurationEstimator->getDuration();
    if (durationUs > maxDurationUs) {
      durationUs = maxDurationUs;
    }

    // get phase current
    mPhaseCurrentEstimator->getPhaseCurrent(&currId, &currIq);

    // calculate error
    errorId = mTargetId - currId;
    errorIq = mTargetIq - currIq;

    // calculate output
    outputUd = mPidD(errorId, durationUs * 1.0e-6f);
    outputUq = mPidQ(errorIq, durationUs * 1.0e-6f);

    // set output
    mFocMotorDriver->setVoltage(outputUd, outputUq);

    mDurationEstimator->recordStart();
  }
}

void MotorCtlCurrent::emergencyStop() { this->stop(); }

bool MotorCtlCurrent::fatalError() { return false; }

void MotorCtlCurrent::setTargetCurrent(float id, float iq) {
  mTargetId = id;
  mTargetIq = iq;

  mParams->setValue(ParamId::MotorCtl_General_TargetCurrentD_RT, mTargetId);
  mParams->setValue(ParamId::MotorCtl_General_TargetCurrentQ_RT, mTargetIq);
}

}  // namespace motorctl
}  // namespace coriander
