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
#include "coriander/os/imutex.h"

namespace coriander {

struct BoardState : public IBoardState {
  BoardState(const BoardState&) = delete;
  BoardState(BoardState&&) = delete;
  virtual BoardState& operator=(const BoardState&) = delete;

  explicit BoardState(
      std::unique_ptr<IStateInitHandler> initHandler,
      std::unique_ptr<IStateStandbyHandler> standbyHandler,
      std::unique_ptr<IStateRunHandler> runHandler,
      std::unique_ptr<IStateErrorHandler> errorHandler,
      std::unique_ptr<IStateCalibrateHandler> calibrateHandler,
      std::unique_ptr<IStateFirmwareUpdateHandler> firmwareUpdateHandler,
      std::shared_ptr<IBoardEvent> event) noexcept;

  virtual void setState(State state) noexcept override;
  virtual State getState() const noexcept override;
  virtual void loop() noexcept override;

 private:
  State mState;
  std::unique_ptr<IStateInitHandler> mStateInitHandler;
  std::unique_ptr<IStateStandbyHandler> mStateStandbyHandler;
  std::unique_ptr<IStateRunHandler> mStateRunHandler;
  std::unique_ptr<IStateErrorHandler> mStateErrorHandler;
  std::unique_ptr<IStateCalibrateHandler> mStateCalibrateHandler;
  std::unique_ptr<IStateFirmwareUpdateHandler> mStateFirmwareUpdateHandler;
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

struct StateInitHandler : public IStateInitHandler {
  StateInitHandler(std::shared_ptr<IAppStatus> appStatus,
                   std::shared_ptr<IBoardEvent> event) noexcept
      : mAppStatus(appStatus), mEvent(event) {}
  virtual void onEnter() noexcept override {
    mAppStatus->setStatus(IAppStatus::Status::Busy);
    mEvent->raiseEvent(IBoardEvent::Event::InitDone);
  }
  virtual void onExit() noexcept override {
    mAppStatus->setStatus(IAppStatus::Status::Ok);
  }
  virtual void onLoop() noexcept override {}

 private:
  std::shared_ptr<IAppStatus> mAppStatus;
  std::shared_ptr<IBoardEvent> mEvent;
};

struct StateStandbyHandler : public IStateStandbyHandler {
  StateStandbyHandler(std::shared_ptr<IAppStatus> appStatus) noexcept
      : mAppStatus(appStatus) {}

  virtual void onEnter() noexcept override {
    mAppStatus->setStatus(IAppStatus::Status::Ok);
  }
  virtual void onExit() noexcept override {}
  virtual void onLoop() noexcept override {}

 private:
  std::shared_ptr<IAppStatus> mAppStatus;
};

struct StateRunHandler : public IStateRunHandler {
  StateRunHandler(std::shared_ptr<IAppStatus> appStatus) noexcept
      : mAppStatus(appStatus) {}

  virtual void onEnter() noexcept override {
    mAppStatus->setStatus(IAppStatus::Status::Ok);
  }
  virtual void onExit() noexcept override {}
  virtual void onLoop() noexcept override {}

 private:
  std::shared_ptr<IAppStatus> mAppStatus;
};

struct StateErrorHandler : public IStateErrorHandler {
  StateErrorHandler(std::shared_ptr<IAppStatus> appStatus) noexcept
      : mAppStatus(appStatus) {}

  virtual void onEnter() noexcept override {
    mAppStatus->setStatus(IAppStatus::Status::Error);
  }
  virtual void onExit() noexcept override {}
  virtual void onLoop() noexcept override {}

 private:
  std::shared_ptr<IAppStatus> mAppStatus;
};

struct StateCalibrateHandler : public IStateCalibrateHandler {
  StateCalibrateHandler(std::shared_ptr<IAppStatus> appStatus) noexcept
      : mAppStatus(appStatus) {}

  virtual void onEnter() noexcept override {
    mAppStatus->setStatus(IAppStatus::Status::Busy);
  }
  virtual void onExit() noexcept override {}
  virtual void onLoop() noexcept override {}

 private:
  std::shared_ptr<IAppStatus> mAppStatus;
};

struct StateFirmwareUpdateHandler : public IStateFirmwareUpdateHandler {
  StateFirmwareUpdateHandler(std::shared_ptr<IAppStatus> appStatus) noexcept
      : mAppStatus(appStatus) {}

  virtual void onEnter() noexcept override {
    mAppStatus->setStatus(IAppStatus::Status::Busy);
  }
  virtual void onExit() noexcept override {}
  virtual void onLoop() noexcept override {}

 private:
  std::shared_ptr<IAppStatus> mAppStatus;
};

}  // namespace coriander
