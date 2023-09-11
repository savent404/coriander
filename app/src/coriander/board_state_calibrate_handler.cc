/**
 * @file board_state_calibrate_handler.cc
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-09-11
 *
 * Copyright 2023 savent_gate
 *
 */
#include "coriander/board_state_calibrate_handler.h"

namespace coriander {

BoardStateCalibrateHandler::BoardStateCalibrateHandler(
    std::shared_ptr<IAppStatus> appStatus,
    std::shared_ptr<MotorCtlCalibrate> motorCtlCalibrate) noexcept
    : mAppStatus(appStatus), mMotorCtlCalibrate(motorCtlCalibrate) {}
void BoardStateCalibrateHandler::onEnter() noexcept {
  mAppStatus->setStatus(IAppStatus::Status::Busy);
  mMotorCtlCalibrate->start();
}

void BoardStateCalibrateHandler::onExit() noexcept {
  mMotorCtlCalibrate->stop();
  mAppStatus->setStatus(IAppStatus::Status::Ok);
}
void BoardStateCalibrateHandler::onLoop() noexcept {
  mMotorCtlCalibrate->loop();
}
}  // namespace coriander
