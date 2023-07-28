#pragma once

#include <memory>

#include "coriander/application/iappstatus.h"
#include "coriander/iboard_state_run_handler.h"

namespace coriander {

struct BoardStateRunHandler : public IBoardStateRunHandler {
  BoardStateRunHandler(std::shared_ptr<IAppStatus> appStatus) noexcept
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
