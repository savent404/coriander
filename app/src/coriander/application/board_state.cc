/**
 * @file board_state.cc
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-07-27
 *
 * Copyright 2023 savent_gate
 *
 */
#include "coriander/board_state.h"

#include "coriander/iboard_state.h"

namespace coriander {
BoardState::BoardState(
    std::unique_ptr<IStateInitHandler> initHandler,
    std::unique_ptr<IStateStandbyHandler> standbyHandler,
    std::unique_ptr<IStateRunHandler> runHandler,
    std::unique_ptr<IStateErrorHandler> errorHandler,
    std::unique_ptr<IStateCalibrateHandler> calibrateHandler,
    std::unique_ptr<IStateFirmwareUpdateHandler> firmwareUpdateHandler) noexcept
    : mState(State::Init),
      mStateInitHandler(std::move(initHandler)),
      mStateStandbyHandler(std::move(standbyHandler)),
      mStateRunHandler(std::move(runHandler)),
      mStateErrorHandler(std::move(errorHandler)),
      mStateCalibrateHandler(std::move(calibrateHandler)),
      mStateFirmwareUpdateHandler(std::move(firmwareUpdateHandler)) {
  if (mStateInitHandler) mStateInitHandler->onEnter();
}

void BoardState::setState(State state) noexcept {
  auto currentStateHandler = getHandler(mState);
  auto nextStateHandler = getHandler(state);

  if (currentStateHandler) currentStateHandler->onExit();
  if (nextStateHandler) nextStateHandler->onEnter();

  // TODO(savent): add a mutex
  mState = state;
}

BoardState::State BoardState::getState() const noexcept { return mState; }

void BoardState::loop() noexcept {
  auto currentStateHandler = getHandler(mState);
  if (currentStateHandler) currentStateHandler->onLoop();
}

}  // namespace coriander
