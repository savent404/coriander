/**
 * @file iprotocol_ctl.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-12
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once

#include "coriander/iboard_event.h"
#include "coriander/iboard_state.h"
#include "coriander/parameters.h"

namespace coriander {

struct IProtocolCtl {
  virtual ~IProtocolCtl() = default;

  virtual void init() noexcept = 0;
  virtual void loop() noexcept = 0;
  virtual void refresh() noexcept = 0;

  // to avoid circular dependency, we use this method to set the
  // dependencies
  virtual void setBoardState(IBoardState *boardState) = 0;
};

}  // namespace coriander
