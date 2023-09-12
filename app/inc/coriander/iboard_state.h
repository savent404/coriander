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

namespace coriander {
struct IBoardState {
  enum class State {
    Init,
    Standby,
    Run,
    Error,
    Calibrate,
    FirmwareUpdate,
    Reboot,
  };

  virtual ~IBoardState() = default;

  /**
   * @brief Set the State object
   *
   * @param state see @c State
   * @note if state need to be changed:
   *  previous state's @c onExit will be called
   *  new state's @c onEnter will be called
   */
  virtual void setState(State state) noexcept = 0;

  /**
   * @brief Get the State object
   *
   * @return State  see @c State
   */
  virtual State getState() const noexcept = 0;

  /**
   * @brief
   *
   */
  virtual void loop() noexcept = 0;
};

}  // namespace coriander
