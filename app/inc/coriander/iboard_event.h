/**
 * @file iboard_event.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-07-28
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once

#include <functional>

namespace coriander {
struct IBoardEvent {
  enum class Event : int {
    BoardInited = 0,      // board init finished
    MotorStart,           // motor start to work
    MotorStop,            // motor stop working
    ParameterUpdate,      // parameter updated, need re-load parameter
    Crap,                 // crap detected
    CrapHandled,          // crap handled, go back to normal state
    CrapUnhandle,         // crap unhandled, go to error state
    CalibrateStart,       // start calibrate
    CalibrateDone,        // calibrate done
    FirmwareStartUpdate,  // start firmware update
    FirmwareUpdateDone,   // firmware update done
    MAX_EVENT,  // max event number, used to check if the event is valid
  };
  using EventCallback = std::function<void(Event)>;

  virtual ~IBoardEvent() = default;

  /**
   * @brief Raise an event
   *
   * @param event see @c Event
   */
  virtual void raiseEvent(Event event) noexcept = 0;

  /**
   * @brief Register an event callback
   *
   * @param event see @c Event
   * @param callback callback function
   * @note allow multiple callback for one event, callback will be called in
   *       the order of registration
   */
  virtual void registerEventCallback(Event event,
                                     EventCallback callback) noexcept = 0;

  /**
   * @brief Lock the event, prevent event from being raised
   */
  virtual void eventLock() = 0;

  /**
   * @brief Unlock the event, allow event to be raised
   */
  virtual void eventUnlock() = 0;
};
}  // namespace coriander
