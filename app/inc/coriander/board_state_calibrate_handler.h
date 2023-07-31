#pragma once

#include <memory>

#include "coriander/application/iappstatus.h"
#include "iboard_state_calibrate_handler.h"

namespace coriander {
using application::IAppStatus;
struct BoardStateCalibrateHandler : public IBoardStateCalibrateHandler {
  BoardStateCalibrateHandler(std::shared_ptr<IAppStatus> appStatus) noexcept
      : mAppStatus(appStatus) {}

  virtual void onEnter() noexcept override {
    mAppStatus->setStatus(IAppStatus::Status::Busy);
  }
  virtual void onExit() noexcept override {}
  virtual void onLoop() noexcept override {}

 private:
  std::shared_ptr<IAppStatus> mAppStatus;
};
}  // namespace coriander