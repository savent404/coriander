/**
 * @file board_state_calibrate_handler.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-05
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once

#include "coriander/application/iappstatus.h"
#include "coriander/iboard_state_calibrate_handler.h"
#include "coriander/motorctl/motor_ctl_calibrate.h"

namespace coriander {

struct BoardStateCalibrateHandler : public IBoardStateCalibrateHandler {
  using IAppStatus = application::IAppStatus;
  using MotorCtlCalibrate = motorctl::MotorCtlCalibrate;
  BoardStateCalibrateHandler(
      std::shared_ptr<IAppStatus> appStatus,
      std::shared_ptr<MotorCtlCalibrate> motorCtlCalibrate) noexcept
      : mAppStatus(appStatus), mMotorCtlCalibrate(motorCtlCalibrate) {}

  virtual void onEnter() noexcept override {
    mAppStatus->setStatus(IAppStatus::Status::Busy);
    mMotorCtlCalibrate->start();
  }

  virtual void onExit() noexcept override {
    mMotorCtlCalibrate->stop();
    mAppStatus->setStatus(IAppStatus::Status::Ok);
  }

  virtual void onLoop() noexcept override { mMotorCtlCalibrate->loop(); }

 private:
  std::shared_ptr<IAppStatus> mAppStatus;
  std::shared_ptr<MotorCtlCalibrate> mMotorCtlCalibrate;
};
}  // namespace coriander