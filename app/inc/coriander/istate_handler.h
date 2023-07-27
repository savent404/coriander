/**
 * @file istate_handler.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-07-27
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once

namespace coriander {

struct IStateHandler {
  virtual ~IStateHandler() = default;

  virtual void onEnter() noexcept = 0;
  virtual void onExit() noexcept = 0;
  virtual void onLoop() noexcept = 0;
};

}  // namespace coriander
