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

#include <zephyr/kernel.h>

#include <memory>

#include "coriander/application/iappstatus.h"
#include "coriander/iboard_event.h"
#include "coriander/iboard_state.h"
#include "coriander/iboard_state_init_handler.h"
#include "coriander/iboard_state_calibrate_handler.h"
#include "coriander/iboard_state_error_handler.h"
#include "coriander/iboard_state_firmware_update_handler.h"
#include "coriander/iboard_state_init_handler.h"
#include "coriander/iboard_state_run_handler.h"
#include "coriander/iboard_state_standby_handler.h"

namespace coriander {

struct BoardState : public IBoardState {
  BoardState(const BoardState&) = delete;
  BoardState(BoardState&&) = delete;
  virtual BoardState& operator=(const BoardState&) = delete;

  explicit BoardState(
      std::unique_ptr<IBoardStateInitHandler> initHandler,
      std::unique_ptr<IBoardStateStandbyHandler> standbyHandler,
      std::unique_ptr<IBoardStateRunHandler> runHandler,
      std::unique_ptr<IBoardStateErrorHandler> errorHandler,
      std::unique_ptr<IBoardStateCalibrateHandler> calibrateHandler,
      std::unique_ptr<IBoardStateFirmwareUpdateHandler> firmwareUpdateHandler,
      std::shared_ptr<IBoardEvent> event) noexcept;

  virtual void setState(State state) noexcept override;
  virtual State getState() const noexcept override;
  virtual void loop() noexcept override;

 private:
  State mState;
  std::unique_ptr<IBoardStateInitHandler> mStateInitHandler;
  std::unique_ptr<IBoardStateStandbyHandler> mStateStandbyHandler;
  std::unique_ptr<IBoardStateRunHandler> mStateRunHandler;
  std::unique_ptr<IBoardStateErrorHandler> mStateErrorHandler;
  std::unique_ptr<IBoardStateCalibrateHandler> mStateCalibrateHandler;
  std::unique_ptr<IBoardStateFirmwareUpdateHandler> mStateFirmwareUpdateHandler;
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

      default:
        return nullptr;
    }
  }
};

}  // namespace coriander
