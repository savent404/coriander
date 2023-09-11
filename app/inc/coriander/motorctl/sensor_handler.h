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
      std::initializer_list<std::shared_ptr<ISensor>> sensors);

  void enable();
  void disable();
  void sync();

 private:
  unsigned mSync;
  std::forward_list<std::shared_ptr<ISensor>> mSensors;
};
}  // namespace motorctl
}  // namespace coriander
