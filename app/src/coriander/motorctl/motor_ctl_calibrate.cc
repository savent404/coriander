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
    std::shared_ptr<IElecAngleEstimator> elecAngleEstimator,
    std::shared_ptr<ParameterBase> param,
    std::shared_ptr<IBoardEvent> boardEvent, std::shared_ptr<ISystick> systick)
    : mMotor(motor),
      mElecAngleEstimator(elecAngleEstimator),
      mParam(param),
      mBoardEvent(boardEvent),
      mSystick(systick),
      mSensorHandler{mElecAngleEstimator} {}

void MotorCtlCalibrate::start() {
  uint16_t dc;

  mSensorHandler.enable();

  mCalibrateVoltage = mParam->getValue<float>("calibrate_voltage"_hash);
  mCalibrateDuration = mParam->getValue<int32_t>("calibrate_duration"_hash);
  mMotorSupplyVoltage = mParam->getValue<float>("motor_supply_voltage"_hash);

  dc = static_cast<uint16_t>(UINT16_MAX * mCalibrateVoltage /
                             mMotorSupplyVoltage);
  mMotor->enable();
  mMotor->setPhaseDutyCycle(dc, 0, 0);
  startTimestamp = mSystick->systick_ms();
}

void MotorCtlCalibrate::stop() {
  mMotor->disable();
  mSensorHandler.disable();
}

void MotorCtlCalibrate::loop() {
  uint32_t current = mSystick->systick_ms();

  mSensorHandler.sync();
  mElecAngleEstimator->calibrate();

  if (current > startTimestamp + mCalibrateDuration) {
    mBoardEvent->raiseEvent(IBoardEvent::Event::CalibrateDone);
  }
}

void MotorCtlCalibrate::emergencyStop() { this->stop(); }

bool MotorCtlCalibrate::fatalError() { return false; }

}  // namespace motorctl
}  // namespace coriander
