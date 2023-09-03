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
#include <memory>
#include <utility>

#include "coriander/iboard_event.h"
#include "coriander/parameters.h"

namespace coriander {
struct BoardCtl {
  using IBoardEvent = coriander::IBoardEvent;
  using Parameter = coriander::Parameter;

  BoardCtl(std::shared_ptr<IBoardEvent> boardEvent,
           std::shared_ptr<Parameter> Parameter)
      : mBoardEvent(boardEvent), mParameter(Parameter) {}

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

  void motorCalibrate() {
    mBoardEvent->eventLock();
    mBoardEvent->raiseEvent(IBoardEvent::Event::CalibrateStart);
    mBoardEvent->eventUnlock();
  }

  void parameterUpdate() {
    mBoardEvent->eventLock();
    mBoardEvent->raiseEvent(IBoardEvent::Event::ParameterUpdate);
    mBoardEvent->eventUnlock();
  }

 private:
  std::shared_ptr<IBoardEvent> mBoardEvent;
  std::shared_ptr<Parameter> mParameter;
};

}  // namespace coriander
