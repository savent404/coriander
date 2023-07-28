#pragma once

#include <memory>

#include "coriander/application/iappstatus.h"
#include "coriander/iboard_state_standby_handler.h"

namespace coriander {
struct BoardStateStandbyHandler : public IBoardStateStandbyHandler {
  BoardStateStandbyHandler(std::shared_ptr<IAppStatus> appStatus) noexcept
      : mAppStatus(appStatus) {}

  virtual void onEnter() noexcept override {
    mAppStatus->setStatus(IAppStatus::Status::Ok);
  }
  virtual void onExit() noexcept override {}
  virtual void onLoop() noexcept override {}

 private:
  std::shared_ptr<IAppStatus> mAppStatus;
};
}  // namespace coriander
