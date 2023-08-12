#pragma once

#include <memory>

#include "coriander/application/iappstatus.h"
#include "coriander/iboard_state_standby_handler.h"
#include "coriander/iprotocol_ctl.h"

namespace coriander {

using application::IAppStatus;
struct BoardStateStandbyHandler : public IBoardStateStandbyHandler {
  BoardStateStandbyHandler(std::shared_ptr<IAppStatus> appStatus,
                           std::shared_ptr<IProtocolCtl> proto) noexcept
      : mAppStatus(appStatus), mProtocolCtl(proto) {}

  virtual void onEnter() noexcept override {
    mProtocolCtl->enable();
    mAppStatus->setStatus(IAppStatus::Status::Ok);
  }
  virtual void onExit() noexcept override {}
  virtual void onLoop() noexcept override { mProtocolCtl->loop(); }

 private:
  std::shared_ptr<IAppStatus> mAppStatus;
  std::shared_ptr<IProtocolCtl> mProtocolCtl;
};
}  // namespace coriander
