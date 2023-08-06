#pragma once

#include <memory>

#include "coriander/application/iappstatus.h"
#include "coriander/iboard_event.h"
#include "coriander/iboard_state_run_handler.h"
#include "coriander/motorctl/imotorctl.h"

namespace coriander {

using application::IAppStatus;
struct BoardStateRunHandler : public IBoardStateRunHandler {
  BoardStateRunHandler(std::shared_ptr<IBoardEvent> boardEvent,
                       std::shared_ptr<motorctl::IMotorCtl> motorCtl,
                       std::shared_ptr<IAppStatus> appStatus) noexcept
      : mBoardEvent{boardEvent}, mMotorCtl{motorCtl}, mAppStatus(appStatus) {}

  virtual void onEnter() noexcept override {
    mAppStatus->setStatus(IAppStatus::Status::Ok);
    mMotorCtl->start();

    if (mMotorCtl->fatalError()) {
      mMotorCtl->stop();
      mAppStatus->setStatus(IAppStatus::Status::Error);
      mBoardEvent->raiseEvent(IBoardEvent::Event::Crap);
    }
  }
  virtual void onExit() noexcept override { mMotorCtl->stop(); }
  virtual void onLoop() noexcept override {
    mMotorCtl->loop();
    if (mMotorCtl->fatalError()) {
      mMotorCtl->stop();
      mAppStatus->setStatus(IAppStatus::Status::Error);
      mBoardEvent->raiseEvent(IBoardEvent::Event::Crap);
    }
  }

 private:
  std::shared_ptr<IBoardEvent> mBoardEvent;
  std::shared_ptr<motorctl::IMotorCtl> mMotorCtl;
  std::shared_ptr<IAppStatus> mAppStatus;
};
}  // namespace coriander
