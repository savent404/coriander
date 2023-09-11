/**
 * @file board_state_run_handler.h
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
#include "coriander/iboard_event.h"
#include "coriander/iboard_state_run_handler.h"
#include "coriander/motorctl/imotorctl.h"

namespace coriander {

struct BoardStateRunHandler : public IBoardStateRunHandler {
  using IAppStatus = application::IAppStatus;
  BoardStateRunHandler(std::shared_ptr<IBoardEvent> boardEvent,
                       std::shared_ptr<motorctl::IMotorCtl> motorCtl,
                       std::shared_ptr<IAppStatus> appStatus) noexcept;

  void onEnter() noexcept;
  void onExit() noexcept;
  void onLoop() noexcept override;

 private:
  std::shared_ptr<IBoardEvent> mBoardEvent;
  std::shared_ptr<motorctl::IMotorCtl> mMotorCtl;
  std::shared_ptr<IAppStatus> mAppStatus;
  int mSyncId;
};
}  // namespace coriander
