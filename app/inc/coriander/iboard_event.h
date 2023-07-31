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
    BoardInited = 0,
    MotorStart,
    MotorStop,
    ParameterUpdate,
    Crap,
    CrapHandled,
    CrapUnhandle,
    CalibrateStart,
    CalibrateDone,
    FirmwareStartUpdate,
    FirmwareUpdateDone,
    MAX_EVENT,
  };
  using EventCallback = std::function<void(Event)>;

  virtual ~IBoardEvent() = default;
  virtual void raiseEvent(Event event) noexcept = 0;
  virtual void registerEventCallback(Event event,
                                     EventCallback callback) noexcept = 0;
};
}  // namespace coriander
