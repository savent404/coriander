/**
 * @file board_state_error_handler.cc
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-09-11
 *
 * Copyright 2023 savent_gate
 *
 */
#include "coriander/board_state_error_handler.h"

namespace coriander {

BoardStateErrorHandler::BoardStateErrorHandler(
    std::shared_ptr<IAppStatus> appStatus) noexcept
    : mAppStatus(appStatus) {}

void BoardStateErrorHandler::onEnter() noexcept {
  mAppStatus->setStatus(IAppStatus::Status::Error);
}

void BoardStateErrorHandler::onExit() noexcept {}
void BoardStateErrorHandler::onLoop() noexcept {}

}  // namespace coriander
