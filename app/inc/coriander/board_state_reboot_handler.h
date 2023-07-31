/**
 * @file board_state_reboot_handler.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-07-31
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once

#include "coriander/iboard_state_reboot_handler.h"

namespace coriander {

struct BoardStateRebootHandler : public IBoardStateRebootHandler {
  BoardStateRebootHandler() noexcept {}
  virtual void onEnter() noexcept override {}
  virtual void onExit() noexcept override {}
  virtual void onLoop() noexcept override {}
};

}  // namespace coriander
