/**
 * @file iboard_state.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-07-27
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once

#include "coriander/istate_handler.h"

namespace coriander {

struct IStateInitHandler : public IStateHandler {};
struct IStateStandbyHandler : public IStateHandler {};
struct IStateRunHandler : public IStateHandler {};
struct IStateErrorHandler : public IStateHandler {};
struct IStateCalibrateHandler : public IStateHandler {};
struct IStateFirmwareUpdateHandler : public IStateHandler {};

struct IBoardState {

  enum class State {
    Init,
    Standby,
    Run,
    Error,
    Calibrate,
    FirmwareUpdate,
  };

  virtual ~IBoardState() = default;

  virtual void setState(State state) noexcept = 0;
  virtual State getState() const noexcept = 0;
  virtual void loop() noexcept = 0;
};

}  // namespace coriander
