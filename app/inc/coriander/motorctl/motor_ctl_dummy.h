/**
 * @file motor_ctl_dummy.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-06
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once
#include <memory>

#include "coriander/base/ilogger.h"
#include "coriander/iboard_event.h"
#include "coriander/motorctl/imotorctl.h"

namespace coriander {
namespace motorctl {
struct MotorCtlDummy : public IMotorCtl {
  MotorCtlDummy(std::shared_ptr<base::ILogger> logger,
                std::shared_ptr<IBoardEvent> boardEvent)
      : mLogger(logger), mBoardEvent(boardEvent) {}

  void start() override { mLogger->log("MotorCtlDummy::start()"); }
  void stop() override { mLogger->log("MotorCtlDummy::stop()"); }
  void loop() override {
    mLogger->log("MotorCtlDummy::loop(): try to stop");
    mBoardEvent->raiseEvent(IBoardEvent::Event::MotorStop);
  }

  void emergencyStop() override { stop(); }
  bool fatalError() override { return false; }

 private:
  std::shared_ptr<base::ILogger> mLogger;
  std::shared_ptr<coriander::IBoardEvent> mBoardEvent;
};
}  // namespace motorctl
}  // namespace coriander
