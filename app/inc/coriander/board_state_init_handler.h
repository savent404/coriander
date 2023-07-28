#pragma once

#include <memory>

#include "coriander/application/iappstatus.h"
#include "coriander/iboard_event.h"
#include "coriander/iboard_state_init_handler.h"

namespace coriander {

struct BoardStateInitHandler : public IBoardStateInitHandler {
  BoardStateInitHandler(std::shared_ptr<IAppStatus> appStatus,
                   std::shared_ptr<IBoardEvent> event) noexcept
      : mAppStatus(appStatus), mEvent(event) {}
  virtual void onEnter() noexcept override {
    mAppStatus->setStatus(IAppStatus::Status::Busy);
    mEvent->raiseEvent(IBoardEvent::Event::InitDone);
  }
  virtual void onExit() noexcept override {
    mAppStatus->setStatus(IAppStatus::Status::Ok);
  }
  virtual void onLoop() noexcept override {}

 private:
  std::shared_ptr<IAppStatus> mAppStatus;
  std::shared_ptr<IBoardEvent> mEvent;
};

}  // namespace coriander