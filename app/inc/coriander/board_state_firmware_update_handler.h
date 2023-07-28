#pragma once

#include <memory>

#include "coriander/application/iappstatus.h"
#include "coriander/iboard_state_firmware_update_handler.h"

namespace coriander {

struct BoardStateFirmwareUpdateHandler : public IBoardStateFirmwareUpdateHandler {
  BoardStateFirmwareUpdateHandler(std::shared_ptr<IAppStatus> appStatus) noexcept
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
