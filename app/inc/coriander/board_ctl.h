/**
 * @file board_ctl.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-12
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once
#include "coriander/iboard_event.h"
#include "coriander/parameters.h"

namespace coriander {
struct BoardCtl {
  using IBoardEvent = coriander::IBoardEvent;
  using ParameterBase = coriander::ParameterBase;

  BoardCtl(std::shared_ptr<IBoardEvent> boardEvent,
           std::shared_ptr<ParameterBase> parameterBase)
      : mBoardEvent(boardEvent), mParameterBase(parameterBase) {}

  void motorEnable() {
    mBoardEvent->eventLock();
    mBoardEvent->raiseEvent(IBoardEvent::Event::MotorStart);
    mBoardEvent->eventUnlock();
  }

  void motorDisable() {
    mBoardEvent->eventLock();
    mBoardEvent->raiseEvent(IBoardEvent::Event::MotorStop);
    mBoardEvent->eventUnlock();
  }

 private:
  std::shared_ptr<IBoardEvent> mBoardEvent;
  std::shared_ptr<ParameterBase> mParameterBase;
};

}  // namespace coriander