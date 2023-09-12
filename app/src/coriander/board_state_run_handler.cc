/**
 * @file board_state_run_handler.cc
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-09-11
 *
 * Copyright 2023 savent_gate
 *
 */
#include "coriander/board_state_run_handler.h"

namespace coriander {

BoardStateRunHandler::BoardStateRunHandler(
    std::shared_ptr<IBoardEvent> boardEvent,
    std::shared_ptr<motorctl::IMotorCtl> motorCtl,
    std::shared_ptr<IAppStatus> appStatus) noexcept
    : mBoardEvent{boardEvent}, mMotorCtl{motorCtl}, mAppStatus(appStatus) {}

void BoardStateRunHandler::onEnter() noexcept {
  mAppStatus->setStatus(IAppStatus::Status::Ok);
  mMotorCtl->start();

  if (mMotorCtl->fatalError()) {
    mMotorCtl->emergencyStop();
    mAppStatus->setStatus(IAppStatus::Status::Error);
    mBoardEvent->raiseEvent(IBoardEvent::Event::Crap);
  }
}

void BoardStateRunHandler::onExit() noexcept { mMotorCtl->stop(); }

void BoardStateRunHandler::onLoop() noexcept {
  mMotorCtl->loop();
  if (mMotorCtl->fatalError()) {
    mMotorCtl->emergencyStop();
    mAppStatus->setStatus(IAppStatus::Status::Error);
    mBoardEvent->raiseEvent(IBoardEvent::Event::Crap);
  }
}

}  // namespace coriander
