/**
 * @file velocity_estimator.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-03
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once

#include <list>

#include "coriander/motorctl/imech_angle_estimator.h"
#include "coriander/motorctl/ivelocity_estimator.h"
#include "coriander/os/isystick.h"
#include "coriander/parameters.h"

namespace coriander {
namespace motorctl {

/**
 * @brief Velocity Estimator based on time windows(default method)
 *
 * @param velocity_sample_window_size<required> int32_t max size of velocity
 * sample window
 * @param velocity_sample_window_time<required> int32_t max time of velocity
 * sample window
 * @param velocity_sample_minimal_duration<required> int32_t minimal duration
 * of velocity sample
 */
struct VelocityEstimator : public IVelocityEstimator {
  using ISystick = coriander::os::ISystick;

  struct RecordItem {
    uint32_t timestamp;
    float angle;
  };

  using Records = std::list<RecordItem>;

  explicit VelocityEstimator(std::shared_ptr<IMechAngleEstimator> mechAngleEstimator,
                             std::shared_ptr<ParameterBase> param,
                             std::shared_ptr<ISystick> systick);
  virtual void enable();
  virtual void disable();
  virtual bool enabled();
  virtual void sync();
  virtual void calibrate();
  virtual bool needCalibrate();
  virtual float getVelocity();

 private:
  std::shared_ptr<IMechAngleEstimator> mMechAngleEstimator;
  std::shared_ptr<ParameterBase> mParam;
  std::shared_ptr<ISystick> mSystick;
  float mVelocity;
  Records mRecords;
  uint32_t mMaxSamples;
  uint32_t mMaxSampleWindowTime;
  uint32_t mMinSampleTime;
  bool mEnabled;
};

}  // namespace motorctl
}  // namespace coriander