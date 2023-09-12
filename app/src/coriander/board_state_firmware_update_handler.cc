/**
 * @file board_state_firmware_update_handler.cc
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-09-11
 *
 * Copyright 2023 savent_gate
 *
 */
#include "coriander/board_state_firmware_update_handler.h"

namespace coriander {

BoardStateFirmwareUpdateHandler::BoardStateFirmwareUpdateHandler(
    std::shared_ptr<IAppStatus> appStatus) noexcept
    : mAppStatus(appStatus) {}

void BoardStateFirmwareUpdateHandler::onEnter() noexcept {
  mAppStatus->setStatus(IAppStatus::Status::Busy);
}

void BoardStateFirmwareUpdateHandler::onExit() noexcept {
  // TODO(savent): implement this
}

void BoardStateFirmwareUpdateHandler::onLoop() noexcept {
  // TODO(savent): implement this
}

}  // namespace coriander
