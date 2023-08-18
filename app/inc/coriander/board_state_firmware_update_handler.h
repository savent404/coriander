/**
 * @file board_state_firmware_update_handler.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief 
 * @date 2023-08-19
 * 
 * Copyright 2023 savent_gate
 * 
 */
#pragma once

#include <memory>

#include "coriander/application/iappstatus.h"
#include "coriander/iboard_state_firmware_update_handler.h"

namespace coriander {

using application::IAppStatus;
struct BoardStateFirmwareUpdateHandler
    : public IBoardStateFirmwareUpdateHandler {
  BoardStateFirmwareUpdateHandler(
      std::shared_ptr<IAppStatus> appStatus) noexcept
      : mAppStatus(appStatus) {}

  void onEnter() noexcept override {
    mAppStatus->setStatus(IAppStatus::Status::Busy);
  }
  void onExit() noexcept override {}
  void onLoop() noexcept override {}

 private:
  std::shared_ptr<IAppStatus> mAppStatus;
};

}  // namespace coriander
