/**
 * @file board_state_error_handler.h
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
#include "coriander/iboard_state_error_handler.h"

namespace coriander {

using application::IAppStatus;
struct BoardStateErrorHandler : public IBoardStateErrorHandler {
  explicit BoardStateErrorHandler(
      std::shared_ptr<IAppStatus> appStatus) noexcept
      : mAppStatus(appStatus) {}

  void onEnter() noexcept override {
    mAppStatus->setStatus(IAppStatus::Status::Error);
  }
  void onExit() noexcept override {}
  void onLoop() noexcept override {}

 private:
  std::shared_ptr<IAppStatus> mAppStatus;
};

}  // namespace coriander
