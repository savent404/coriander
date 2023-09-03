/**
 * @file motor_ctl_calibrate.cc
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-04
 *
 * Copyright 2023 savent_gate
 *
 */
#include "coriander/motorctl/motor_ctl_calibrate.h"

#include "coriander/iboard_event.h"

namespace coriander {
namespace motorctl {
MotorCtlCalibrate::MotorCtlCalibrate(
    std::shared_ptr<IBldcDriver> motor,
    std::shared_ptr<IPhaseCurrentEstimator> phaseCurrentEstimator,
    std::shared_ptr<IElecAngleEstimator> elecAngleEstimator,
    std::shared_ptr<Parameter> param, std::shared_ptr<IBoardEvent> boardEvent,
    std::shared_ptr<ISystick> systick, std::shared_ptr<ILogger> logger,
    std::shared_ptr<IParamReqValidator> paramReqValidator)
    : mMotor(motor),
      mPhaseCurrentEstimator(phaseCurrentEstimator),
      mElecAngleEstimator(elecAngleEstimator),
      mParam(param),
      mBoardEvent(boardEvent),
      mSystick(systick),
      mLogger(logger),
      mSensorHandler{phaseCurrentEstimator, mElecAngleEstimator} {
  paramReqValidator->addParamReq(this);
}

void MotorCtlCalibrate::start() {
  mSensorHandler.enable();

  mCalibrateVoltage =
      mParam->getValue<float>(ParamId::MotorCtl_Calibrate_CaliVoltage);
  mMotorSupplyVoltage =
      mParam->getValue<float>(ParamId::MotorCtl_MotorDriver_SupplyVoltage);

  seekCalibrateItem();
}

void MotorCtlCalibrate::stop() {
  mMotor->disable();
  mSensorHandler.disable();
}

void MotorCtlCalibrate::loop() {
  uint32_t current = mSystick->systick_ms();
  bool expired = current > startTimestamp + mCalibrateDuration;

  mSensorHandler.sync();

  switch (mState) {
    case State::Calibrate_Idle:
      break;
    case State::Calibrate_PhaseCurrent:
      mPhaseCurrentEstimator->calibrate();
      break;
    case State::Calibrate_ElecAngle:
      mElecAngleEstimator->calibrate();
      break;
  }

  if (expired) {
    exitState();
    seekCalibrateItem();
  }
}

void MotorCtlCalibrate::emergencyStop() { this->stop(); }

bool MotorCtlCalibrate::fatalError() { return false; }

void MotorCtlCalibrate::enterState(State state) {
  switch (state) {
    case State::Calibrate_Idle:
      break;
    case State::Calibrate_PhaseCurrent:
      mCalibrateDuration = 256;  // fixed 256ms
      mMotor->enable();
      mMotor->setPhaseDutyCycle(0, 0, 0);
      mLogger->log("Phase current calibration start");
      break;
    case State::Calibrate_ElecAngle: {
      uint16_t dc = static_cast<uint16_t>(UINT16_MAX * mCalibrateVoltage /
                                          mMotorSupplyVoltage);
      mCalibrateDuration =
          mParam->getValue<int32_t>(ParamId::MotorCtl_Calibrate_CaliDuration);
      mMotor->enable();
      mMotor->setPhaseDutyCycle(dc, 0, 0);
      mLogger->log("Electrical angle calibration start");
    } break;
  }
  // start timer
  startTimestamp = mSystick->systick_ms();
  mState = state;
}

void MotorCtlCalibrate::exitState() {
  switch (mState) {
    case State::Calibrate_Idle:
      break;
    case State::Calibrate_PhaseCurrent:
      mMotor->disable();
      mLogger->log("Phase current calibration done");
      break;
    case State::Calibrate_ElecAngle:
      mMotor->disable();
      mLogger->log("Electrical angle calibration done");
      break;
  }
  mState = State::Calibrate_Idle;
}

void MotorCtlCalibrate::seekCalibrateItem() {
  if (mPhaseCurrentEstimator->needCalibrate()) {
    enterState(State::Calibrate_PhaseCurrent);
  } else if (mElecAngleEstimator->needCalibrate()) {
    enterState(State::Calibrate_ElecAngle);
  } else {
    mBoardEvent->raiseEvent(IBoardEvent::Event::CalibrateDone);
  }
}

}  // namespace motorctl
}  // namespace coriander
