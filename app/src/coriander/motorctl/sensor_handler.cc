/**
 * @file sensor_handler.cc
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-09-11
 *
 * Copyright 2023 savent_gate
 *
 */
#include "coriander/motorctl/sensor_handler.h"

namespace coriander {

namespace motorctl {
SensorHandler::SensorHandler(
    std::initializer_list<std::shared_ptr<ISensor>> sensors)
    : mSync{0} {
  for (auto sensor : sensors) {
    mSensors.push_front(sensor);
  }
}

void SensorHandler::enable() {
  mSync = 0;
  for (auto &&sensor : mSensors) {
    sensor->reset();
    if (!sensor->enabled()) {
      sensor->enable();
    }
  }
}

void SensorHandler::disable() {
  for (auto &&sensor : mSensors) {
    if (sensor->enabled()) {
      sensor->disable();
    }
  }
}

void SensorHandler::sync() {
  mSync++;
  for (auto &&sensor : mSensors) {
    if (sensor->needSync(mSync)) {
      sensor->sync();
    }
  }
}

}  // namespace motorctl

}  // namespace coriander
