/**
 * @file board_state_init_handler.cc
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-09-11
 *
 * Copyright 2023 savent_gate
 *
 */
#include "coriander/board_state_init_handler.h"

namespace coriander {
BoardStateInitHandler::BoardStateInitHandler(
    std::shared_ptr<IAppStatus> appStatus, std::shared_ptr<IBoardEvent> event,
    std::shared_ptr<application::Diagnosis> diagnosis,
    std::shared_ptr<base::ILogger> logger,
    std::shared_ptr<IParamReqValidator> paramReqValidator) noexcept
    : mAppStatus(appStatus),
      mEvent(event),
      mDiagnosis(diagnosis),
      mLogger(logger),
      mParamReqValidator(paramReqValidator) {
  if (mDiagnosis) {
    mDiagnosis->addDiagInspector(
        [this](application::Diagnosis::DiagDevId id,
               application::Diagnosis::DeviceStatus status) {
          if (initDone) {
            return;
          }

          if (status == application::Diagnosis::DeviceStatus::Error) {
            CORIANDER_LOG_ERROR(mLogger,
                                "BoardStateInitHandler: diagnosis error, id:{}",
                                static_cast<int>(id));
            mEvent->raiseEvent(IBoardEvent::Event::Crap);
          }
        });
  } else {
    mLogger->log("BoardStateInitHandler: diagnosis is null");
  }
}

void BoardStateInitHandler::onEnter() noexcept {
  mAppStatus->setStatus(IAppStatus::Status::Busy);

  // do parma check, if any error, goto error state
  if (mParamReqValidator->validate() == false) {
    mEvent->raiseEvent(IBoardEvent::Event::Crap);
    return;
  }

  // do diagnosis, if any error, goto error state
  mDiagnosis->inspect();
}

void BoardStateInitHandler::onExit() noexcept {
  initDone = true;
  mAppStatus->setStatus(IAppStatus::Status::Ok);
}
void BoardStateInitHandler::onLoop() noexcept {
  // no need to do anything, go to next state
  mEvent->raiseEvent(IBoardEvent::Event::BoardInited);
}

}  // namespace coriander
