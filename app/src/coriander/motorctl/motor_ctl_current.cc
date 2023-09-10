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

#include <math.h>

#include "coriander/base/jscope.h"
#include "coriander/motorctl/foc.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

#if CONFIG_JSCOPE_ENABLE
ATTR_JSCOPE static float _dCurrCtlElecAngle = 0.0f;
ATTR_JSCOPE static float _dCurrCtlTargetId = 0.0f;
ATTR_JSCOPE static float _dCurrCtlTargetIq = 0.0f;
ATTR_JSCOPE static float _dCurrCtlCurrId = 0.0f;
ATTR_JSCOPE static float _dCurrCtlCurrIq = 0.0f;
ATTR_JSCOPE static float _dCurrCtlErrId = 0.0f;
ATTR_JSCOPE static float _dCurrCtlErrIq = 0.0f;
ATTR_JSCOPE static float _dCurrCtlOutputUd = 0.0f;
ATTR_JSCOPE static float _dCurrCtlOutputUq = 0.0f;
#endif

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
  mSensorHandler.enable();
  mDurationEstimator->recordStart();
  mDurationTimeout->reset();
  mFocMotorDriver->enable();
}

void MotorCtlCurrent::stop() {
  mSensorHandler.disable();
  mFocMotorDriver->disable();
}

void MotorCtlCurrent::loop() {
  const uint32_t maxDurationUs = 1'000;
  uint32_t durationUs;
  float errorId, errorIq;
  float currId, currIq;
  float Ialpha, Ibeta;
  float thetaRad, sinTheta, cosTheta;
  float outputUd, outputUq;

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

    // get phase current
    mPhaseCurrentEstimator->getPhaseCurrent(&Ialpha, &Ibeta);
    thetaRad = mElecAngleEstimator->getElectricalAngle() / 180.0f * M_PI;
    sinTheta = sinf(thetaRad);
    cosTheta = cosf(thetaRad);
    foc::park(Ialpha, Ibeta, sinTheta, cosTheta, &currId, &currIq);

    currId = mIdLpf(currId, durationUs * 1.0e-6f);
    currIq = mIqLpf(currIq, durationUs * 1.0e-6f);

    // calculate error
    errorId = mTargetId - currId;
    errorIq = mTargetIq - currIq;

    // calculate output
    outputUd = mPidD(errorId, durationUs * 1.0e-6f);
    outputUq = mPidQ(errorIq, durationUs * 1.0e-6f);

    // set output
    mFocMotorDriver->setVoltage(outputUd, outputUq);

    mDurationEstimator->recordStart();
#if CONFIG_JSCOPE_ENABLE
    _dCurrCtlElecAngle = mElecAngleEstimator->getElectricalAngle();
    _dCurrCtlTargetId = mTargetId * 1000.0f;
    _dCurrCtlTargetIq = mTargetIq * 1000.0f;
    _dCurrCtlCurrId = currId * 1000.0f;
    _dCurrCtlCurrIq = currIq * 1000.0f;
    _dCurrCtlErrId = errorId * 1000.0f;
    _dCurrCtlErrIq = errorIq * 1000.0f;
    _dCurrCtlOutputUd = outputUd * 1000.0f;
    _dCurrCtlOutputUq = outputUq * 1000.0f;
#endif
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
