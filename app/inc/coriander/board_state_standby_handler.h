/**
 * @file board_state_standby_handler.h
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
#include "coriander/iboard_state_standby_handler.h"
#include "coriander/iprotocol_ctl.h"

namespace coriander {

struct BoardStateStandbyHandler : public IBoardStateStandbyHandler {
  using IAppStatus = application::IAppStatus;
  BoardStateStandbyHandler(std::shared_ptr<IAppStatus> appStatus,
                           std::shared_ptr<IShellCtl> proto) noexcept;
  void onEnter() noexcept override;
  void onExit() noexcept override;
  void onLoop() noexcept override;

 private:
  std::shared_ptr<IAppStatus> mAppStatus;
  std::shared_ptr<IShellCtl> mProtocolCtl;
};
}  // namespace coriander
