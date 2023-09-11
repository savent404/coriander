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

struct BoardStateInitHandler : public IBoardStateInitHandler {
  using IAppStatus = application::IAppStatus;
  BoardStateInitHandler(
      std::shared_ptr<IAppStatus> appStatus, std::shared_ptr<IBoardEvent> event,
      std::shared_ptr<application::Diagnosis> diagnosis,
      std::shared_ptr<base::ILogger> logger,
      std::shared_ptr<IParamReqValidator> paramReqValidator) noexcept;

  void onEnter() noexcept override;
  void onExit() noexcept override;
  void onLoop() noexcept override;

 private:
  std::shared_ptr<IAppStatus> mAppStatus;
  std::shared_ptr<IBoardEvent> mEvent;
  std::shared_ptr<application::Diagnosis> mDiagnosis;
  std::shared_ptr<base::ILogger> mLogger;
  std::shared_ptr<IParamReqValidator> mParamReqValidator;
  bool initDone = false;
};

}  // namespace coriander
