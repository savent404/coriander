/**
 * @file board_state_standby_handler.cc
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-09-11
 *
 * Copyright 2023 savent_gate
 *
 */
#include "coriander/board_state_standby_handler.h"

namespace coriander {

BoardStateStandbyHandler::BoardStateStandbyHandler(
    std::shared_ptr<IAppStatus> appStatus,
    std::shared_ptr<IShellCtl> proto) noexcept
    : mAppStatus(appStatus), mProtocolCtl(proto) {}

void BoardStateStandbyHandler::onEnter() noexcept {
  mProtocolCtl->enable();
  mAppStatus->setStatus(IAppStatus::Status::Ok);
}
void BoardStateStandbyHandler::onExit() noexcept {}
void BoardStateStandbyHandler::onLoop() noexcept { mProtocolCtl->loop(); }
}  // namespace coriander
