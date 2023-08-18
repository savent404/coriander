/**
 * @file sensor_handler.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-06
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once

#include <forward_list>
#include <initializer_list>
#include <memory>

#include "coriander/motorctl/isensor.h"

namespace coriander {
namespace motorctl {
struct SensorHandler {
  explicit SensorHandler(
      std::initializer_list<std::shared_ptr<ISensor>> sensors)
      : mSync{0} {
    for (auto sensor : sensors) {
      mSensors.push_front(sensor);
    }
  }

  void enable() {
    mSync = 0;
    for (auto &&sensor : mSensors) {
      sensor->reset();
      if (!sensor->enabled()) {
        sensor->enable();
      }
    }
  }
  void disable() {
    for (auto &&sensor : mSensors) {
      if (sensor->enabled()) {
        sensor->disable();
      }
    }
  }
  void sync() {
    mSync++;
    for (auto &&sensor : mSensors) {
      if (sensor->needSync(mSync)) {
        sensor->sync();
      }
    }
  }

 private:
  unsigned mSync;
  std::forward_list<std::shared_ptr<ISensor>> mSensors;
};
}  // namespace motorctl
}  // namespace coriander
