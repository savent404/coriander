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

struct Encoder : public motorctl::IEncoder {
  virtual unsigned getEncoderCount();
  virtual unsigned getEncoderCountPerRound();
  virtual int getOverflowCount();
  virtual void enable();
  virtual void disable();
  virtual bool enabled();
  virtual void sync();

  /**
   * @brief set the sensor to zero, calibrate offset
   */
  virtual void calibrate();
  virtual bool needCalibrate();
};
}  // namespace zephyr
}  // namespace motorctl
}  // namespace coriander
