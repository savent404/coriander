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

namespace coriander {
BoardState::BoardState(
    std::unique_ptr<IBoardStateInitHandler> initHandler,
    std::unique_ptr<IBoardStateStandbyHandler> standbyHandler,
    std::unique_ptr<IBoardStateRunHandler> runHandler,
    std::unique_ptr<IBoardStateErrorHandler> errorHandler,
    std::unique_ptr<IBoardStateCalibrateHandler> calibrateHandler,
    std::unique_ptr<IBoardStateFirmwareUpdateHandler> firmwareUpdateHandler,
    std::unique_ptr<IBoardStateRebootHandler> rebootHandler,
    std::shared_ptr<IBoardEvent> event) noexcept
    : mState(State::Init),
      mStateInitHandler(std::move(initHandler)),
      mStateStandbyHandler(std::move(standbyHandler)),
      mStateRunHandler(std::move(runHandler)),
      mStateErrorHandler(std::move(errorHandler)),
      mStateCalibrateHandler(std::move(calibrateHandler)),
      mStateFirmwareUpdateHandler(std::move(firmwareUpdateHandler)),
      mStateRebootHandler(std::move(rebootHandler)),
      mEvent(event) {
  event->registerEventCallback(
      IBoardEvent::Event::InitDone,
      [this](IBoardEvent::Event event) { setState(State::Standby); });
  event->registerEventCallback(
      IBoardEvent::Event::DoRun,
      [this](IBoardEvent::Event event) { setState(State::Run); });
  event->registerEventCallback(
      IBoardEvent::Event::RunDone,
      [this](IBoardEvent::Event event) { setState(State::Standby); });
  event->registerEventCallback(
      IBoardEvent::Event::DoError,
      [this](IBoardEvent::Event event) { setState(State::Error); });
  event->registerEventCallback(
      IBoardEvent::Event::DoCalibrate,
      [this](IBoardEvent::Event event) { setState(State::Calibrate); });
  event->registerEventCallback(
      IBoardEvent::Event::CalibrateDone,
      [this](IBoardEvent::Event event) { setState(State::Standby); });
  event->registerEventCallback(
      IBoardEvent::Event::DoFirmwareUpdate,
      [this](IBoardEvent::Event event) { setState(State::FirmwareUpdate); });
  event->registerEventCallback(
      IBoardEvent::Event::FirmwareUpdateDone,
      [this](IBoardEvent::Event event) { setState(State::Reboot); });
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
