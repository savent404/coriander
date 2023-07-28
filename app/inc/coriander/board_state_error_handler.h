#pragma once

#include <memory>

#include "coriander/application/iappstatus.h"
#include "coriander/iboard_state_error_handler.h"

namespace coriander {

struct BoardStateErrorHandler : public IBoardStateErrorHandler {
  BoardStateErrorHandler(std::shared_ptr<IAppStatus> appStatus) noexcept
      : mAppStatus(appStatus) {}

  virtual void onEnter() noexcept override {
    mAppStatus->setStatus(IAppStatus::Status::Error);
  }
  virtual void onExit() noexcept override {}
  virtual void onLoop() noexcept override {}

 private:
  std::shared_ptr<IAppStatus> mAppStatus;
};

}  // namespace coriander
