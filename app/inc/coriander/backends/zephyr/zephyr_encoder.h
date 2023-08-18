/**
 * @file zephyr_encoder.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-07
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once
#include "coriander/motorctl/iencoder.h"

namespace coriander {
namespace motorctl {
namespace zephyr {

// TODO(savent): need to implement
struct Encoder : public motorctl::IEncoder {
  virtual unsigned getEncoderCount() { return 0; }
  virtual unsigned getEncoderCountPerRound() { return 0; }
  virtual int getOverflowCount() { return 0; }
  virtual void enable() {}
  virtual void disable() {}
  virtual bool enabled() { return false; }
  virtual void sync() {}

  /**
   * @brief set the sensor to zero, calibrate offset
   */
  virtual void calibrate() {}
  virtual bool needCalibrate() { return false; }
};
}  // namespace zephyr
}  // namespace motorctl
}  // namespace coriander
