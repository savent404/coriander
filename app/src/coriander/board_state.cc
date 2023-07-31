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
  mEvent->registerEventCallback(
      IBoardEvent::Event::BoardInited, [this](IBoardEvent::Event event) {
        switch (mState) {
          case State::Init:
            setState(State::Standby);
            break;
          default:
            mEvent->raiseEvent(IBoardEvent::Event::Crap);
            break;
        }
      });

  mEvent->registerEventCallback(
      IBoardEvent::Event::MotorStart, [this](IBoardEvent::Event event) {
        switch (mState) {
          case State::Standby:
            setState(State::Run);
            break;
          default:
            mEvent->raiseEvent(IBoardEvent::Event::Crap);
            break;
        }
      });

  mEvent->registerEventCallback(
      IBoardEvent::Event::MotorStop, [this](IBoardEvent::Event event) {
        switch (mState) {
          case State::Run:
            setState(State::Standby);
            break;
          default:
            mEvent->raiseEvent(IBoardEvent::Event::Crap);
            break;
        }
      });

  mEvent->registerEventCallback(
      IBoardEvent::Event::ParameterUpdate, [this](IBoardEvent::Event event) {
        switch (mState) {
          case State::Run:
            // if we are in run state, we need to
            // reload
            setState(State::Standby);
            loop();  // not sure if this is necessary
            setState(State::Run);
            break;
          case State::Standby:
            break;
          default:
            mEvent->raiseEvent(IBoardEvent::Event::Crap);
            break;
        }
      });

  mEvent->registerEventCallback(
      IBoardEvent::Event::Crap,
      [this](IBoardEvent::Event event) { setState(State::Error); });

  mEvent->registerEventCallback(
      IBoardEvent::Event::CrapHandled,
      [this](IBoardEvent::Event event) { setState(State::Standby); });

  mEvent->registerEventCallback(
      IBoardEvent::Event::CrapUnhandle,
      [this](IBoardEvent::Event event) { setState(State::Reboot); });

  mEvent->registerEventCallback(
      IBoardEvent::Event::CalibrateStart, [this](IBoardEvent::Event event) {
        switch (mState) {
          case State::Standby:
            setState(State::Calibrate);
            break;
          default:
            mEvent->raiseEvent(IBoardEvent::Event::Crap);
            break;
        }
      });
  mEvent->registerEventCallback(
      IBoardEvent::Event::CalibrateDone, [this](IBoardEvent::Event event) {
        switch (mState) {
          case State::Calibrate:
            setState(State::Standby);
            break;
          default:
            mEvent->raiseEvent(IBoardEvent::Event::Crap);
            break;
        }
      });
  mEvent->registerEventCallback(
      IBoardEvent::Event::FirmwareStartUpdate,
      [this](IBoardEvent::Event event) {
        switch (mState) {
          case State::Standby:
            setState(State::FirmwareUpdate);
            break;
          default:
            mEvent->raiseEvent(IBoardEvent::Event::Crap);
            break;
        }
      });

  mEvent->registerEventCallback(
      IBoardEvent::Event::FirmwareUpdateDone, [this](IBoardEvent::Event event) {
        switch (mState) {
          case State::FirmwareUpdate:
            setState(State::Standby);
            break;
          default:
            mEvent->raiseEvent(IBoardEvent::Event::Crap);
            break;
        }
      });

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
