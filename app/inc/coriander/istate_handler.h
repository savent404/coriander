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

  /**
   * @brief Called when the state is entered
   *
   * @note this function called in the thread
   *       which called @c IBoardEvent::raiseEvent
   */
  virtual void onEnter() noexcept = 0;

  /**
   * @brief Called when the state is exited
   *
   * @note this function called in the thread
   *       which called @c IBoardEvent::raiseEvent
   */
  virtual void onExit() noexcept = 0;

  /**
   * @brief Called when the state is looped
   *
   * @note this function called in self thread
   */
  virtual void onLoop() noexcept = 0;
};

}  // namespace coriander
