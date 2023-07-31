/**
 * @file board_state.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-07-27
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once

#include <memory>

#include "coriander/application/iappstatus.h"
#include "coriander/iboard_event.h"
#include "coriander/iboard_state.h"
#include "coriander/iboard_state_calibrate_handler.h"
#include "coriander/iboard_state_error_handler.h"
#include "coriander/iboard_state_firmware_update_handler.h"
#include "coriander/iboard_state_init_handler.h"
#include "coriander/iboard_state_reboot_handler.h"
#include "coriander/iboard_state_run_handler.h"
#include "coriander/iboard_state_standby_handler.h"

namespace coriander {

struct BoardState : public IBoardState {
  BoardState(const BoardState&) = delete;
  BoardState(BoardState&&) = delete;
  virtual BoardState& operator=(const BoardState&) = delete;

  explicit BoardState(
      std::shared_ptr<IBoardStateInitHandler> initHandler,
      std::shared_ptr<IBoardStateStandbyHandler> standbyHandler,
      std::shared_ptr<IBoardStateRunHandler> runHandler,
      std::shared_ptr<IBoardStateErrorHandler> errorHandler,
      std::shared_ptr<IBoardStateCalibrateHandler> calibrateHandler,
      std::shared_ptr<IBoardStateFirmwareUpdateHandler> firmwareUpdateHandler,
      std::shared_ptr<IBoardStateRebootHandler> rebootHandler,
      std::shared_ptr<IBoardEvent> event) noexcept;

  virtual void setState(State state) noexcept override;
  virtual State getState() const noexcept override;
  virtual void loop() noexcept override;

 private:
  State mState;
  std::shared_ptr<IBoardStateInitHandler> mStateInitHandler;
  std::shared_ptr<IBoardStateStandbyHandler> mStateStandbyHandler;
  std::shared_ptr<IBoardStateRunHandler> mStateRunHandler;
  std::shared_ptr<IBoardStateErrorHandler> mStateErrorHandler;
  std::shared_ptr<IBoardStateCalibrateHandler> mStateCalibrateHandler;
  std::shared_ptr<IBoardStateFirmwareUpdateHandler> mStateFirmwareUpdateHandler;
  std::shared_ptr<IBoardStateRebootHandler> mStateRebootHandler;
  std::shared_ptr<IBoardEvent> mEvent;

  IStateHandler* getHandler(State s) {
    switch (s) {
      case State::Init:
        return mStateInitHandler.get();
      case State::Standby:
        return mStateStandbyHandler.get();
      case State::Run:
        return mStateRunHandler.get();
      case State::Error:
        return mStateErrorHandler.get();
      case State::Calibrate:
        return mStateCalibrateHandler.get();
      case State::FirmwareUpdate:
        return mStateFirmwareUpdateHandler.get();
      case State::Reboot:
        return mStateRebootHandler.get();
      default:
        return nullptr;
    }
  }
};

}  // namespace coriander
