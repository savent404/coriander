/**
 * @file encoder_mech_angle_estimator.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-03
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once

#include "coriander/base/const_hash.h"
#include "coriander/motorctl/iencoder.h"
#include "coriander/motorctl/imech_angle_estimator.h"
#include "coriander/parameters.h"

namespace coriander {
namespace motorctl {

using namespace coriander::base;

/**
 * @brief Mechanical angle estimator based on encoder
 *
 * @param mech_angle_offset       float   mechanical angle offset
 * @param persist_raw_mech_angle  float   persist mechanical angle, used to
 *                                        after power off
 */
struct EncoderMechAngleEstimator : public IMechAngleEstimator {
  EncoderMechAngleEstimator(IEncoder* encoder, ParameterBase* param) noexcept;

  virtual void enable();
  virtual void disable();
  virtual bool enabled();
  virtual void sync();
  virtual void calibrate();
  virtual bool needCalibrate();
  virtual float getMechanicalAngle() noexcept;

 private:
  IEncoder* mEncoder;
  ParameterBase* mParam;
  float mRawMechAngle;
  float mPersistOffset;
  float mMechAngleOffset;
  float mMechAngle;
  float mNeedCalibrate;
};
}  // namespace motorctl
}  // namespace coriander