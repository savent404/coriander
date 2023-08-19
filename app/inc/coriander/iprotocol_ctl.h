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

namespace coriander {

struct IProtocolCtl {
  virtual ~IProtocolCtl() = default;

  virtual void enable() noexcept = 0;
  virtual void disable() noexcept = 0;
  virtual void loop() noexcept = 0;
};

// alternative interface for debug purpose
struct IShellCtl : IProtocolCtl {};

}  // namespace coriander
