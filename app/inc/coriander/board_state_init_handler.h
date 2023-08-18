/**
 * @file board_state_init_handler.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief 
 * @date 2023-08-19
 * 
 * Copyright 2023 savent_gate
 * 
 */
#pragma once

#include <memory>

#include "coriander/application/diagnosis.h"
#include "coriander/application/iappstatus.h"
#include "coriander/base/ilogger.h"
#include "coriander/base/loggerstream.h"
#include "coriander/iboard_event.h"
#include "coriander/iboard_state_init_handler.h"
#include "coriander/parameter_requirements.h"

namespace coriander {

using application::IAppStatus;
struct BoardStateInitHandler : public IBoardStateInitHandler {
  BoardStateInitHandler(
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
  void onEnter() noexcept override {
    mAppStatus->setStatus(IAppStatus::Status::Busy);

    // do parma check, if any error, goto error state
    if (mParamReqValidator->validate() == false) {
      mEvent->raiseEvent(IBoardEvent::Event::Crap);
      return;
    }

    // do diagnosis, if any error, goto error state
    mDiagnosis->inspect();
  }
  void onExit() noexcept override {
    initDone = true;
    mAppStatus->setStatus(IAppStatus::Status::Ok);
  }
  void onLoop() noexcept override {
    // no need to do anything, go to next state
    mEvent->raiseEvent(IBoardEvent::Event::BoardInited);
  }

 private:
  std::shared_ptr<IAppStatus> mAppStatus;
  std::shared_ptr<IBoardEvent> mEvent;
  std::shared_ptr<application::Diagnosis> mDiagnosis;
  std::shared_ptr<base::ILogger> mLogger;
  std::shared_ptr<IParamReqValidator> mParamReqValidator;
  bool initDone = false;
};

}  // namespace coriander
