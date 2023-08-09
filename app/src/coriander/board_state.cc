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
    std::shared_ptr<IBoardStateInitHandler> initHandler,
    std::shared_ptr<IBoardStateStandbyHandler> standbyHandler,
    std::shared_ptr<IBoardStateRunHandler> runHandler,
    std::shared_ptr<IBoardStateErrorHandler> errorHandler,
    std::shared_ptr<IBoardStateCalibrateHandler> calibrateHandler,
    std::shared_ptr<IBoardStateFirmwareUpdateHandler> firmwareUpdateHandler,
    std::shared_ptr<IBoardStateRebootHandler> rebootHandler,
    std::shared_ptr<IBoardEvent> event, std::unique_ptr<ISemaphore> semaphore,
    std::unique_ptr<IThread> thread) noexcept
    : mState(State::Init),
      mStateInitHandler(initHandler),
      mStateStandbyHandler(standbyHandler),
      mStateRunHandler(runHandler),
      mStateErrorHandler(errorHandler),
      mStateCalibrateHandler(calibrateHandler),
      mStateFirmwareUpdateHandler(firmwareUpdateHandler),
      mStateRebootHandler(rebootHandler),
      mEvent(event),
      mSemaphore(std::move(semaphore)),
      mThreadInfo(std::move(thread)) {
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

  enterCritical();

  if (currentStateHandler) currentStateHandler->onExit();
  if (nextStateHandler) nextStateHandler->onEnter();
  mState = state;

  exitCritical();
}

BoardState::State BoardState::getState() const noexcept { return mState; }

void BoardState::loop() noexcept {
  auto currentStateHandler = getHandler(mState);
  if (currentStateHandler) {
    enterCritical();
    currentStateHandler->onLoop();
    exitCritical();
  }
}

void BoardState::enterCritical() noexcept {
  if (threadInCritical == mThreadInfo->currentThreadId()) {
    return;
  }
  if (mIsCritical) {
    // someone else is in critical section
    mCriticalCount++;
    mSemaphore->wait();
  } else {
    // the first one enters critical section
    mIsCritical = true;
  }
  threadInCritical = mThreadInfo->currentThreadId();
}

void BoardState::exitCritical() noexcept {
  threadInCritical = 0;
  if (mCriticalCount > 0) {
    // leave critical section to someone else
    mCriticalCount--;
    mSemaphore->post();
  } else {
    // the last one leaves critical section
    mIsCritical = false;
  }
}

}  // namespace coriander
