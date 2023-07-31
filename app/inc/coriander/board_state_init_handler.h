#pragma once

#include <memory>

#include "coriander/application/diagnosis.h"
#include "coriander/application/iappstatus.h"
#include "coriander/base/ilogger.h"
#include "coriander/base/loggerstream.h"
#include "coriander/iboard_event.h"
#include "coriander/iboard_state_init_handler.h"

namespace coriander {

using application::IAppStatus;
struct BoardStateInitHandler : public IBoardStateInitHandler {
  BoardStateInitHandler(std::shared_ptr<IAppStatus> appStatus,
                        std::shared_ptr<IBoardEvent> event,
                        std::shared_ptr<application::Diagnosis> diagnosis,
                        std::shared_ptr<base::ILogger> logger) noexcept
      : mAppStatus(appStatus),
        mEvent(event),
        mDiagnosis(diagnosis),
        mLogger(logger) {
    if (mDiagnosis) {
      mDiagnosis->addDiagInspector(
          [this](application::Diagnosis::DiagDevId id,
                 application::Diagnosis::DeviceStatus status) {
            if (initDone) {
              return;
            }

            if (status == application::Diagnosis::DeviceStatus::Error) {
              auto os = base::LoggerStream(mLogger);
              os << "BoardStateInitHandler: diagnosis error, id:"
                 << static_cast<int>(id) << std::endl;
              mEvent->raiseEvent(IBoardEvent::Event::Crap);
            }
          });
    } else {
      mLogger->log("BoardStateInitHandler: diagnosis is null");
    }
  }
  virtual void onEnter() noexcept override {
    mAppStatus->setStatus(IAppStatus::Status::Busy);

    // do diagnosis, if any error, goto error state
    mDiagnosis->inspect();
  }
  virtual void onExit() noexcept override {
    initDone = true;
    mAppStatus->setStatus(IAppStatus::Status::Ok);
  }
  virtual void onLoop() noexcept override {
    // no need to do anything, go to next state
    mEvent->raiseEvent(IBoardEvent::Event::BoardInited);
  }

 private:
  std::shared_ptr<IAppStatus> mAppStatus;
  std::shared_ptr<IBoardEvent> mEvent;
  std::shared_ptr<application::Diagnosis> mDiagnosis;
  std::shared_ptr<base::ILogger> mLogger;
  bool initDone = false;
};

}  // namespace coriander