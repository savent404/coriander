/**
 * @file velocity_estimator.cc
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-03
 *
 * Copyright 2023 savent_gate
 *
 */
#include "coriander/motorctl/velocity_estimator.h"

namespace coriander {
namespace motorctl {

VelocityEstimator::VelocityEstimator(IMechAngleEstimator* mechAngleEstimator,
                                     ParameterBase* param, ISystick* systick)
    : mMechAngleEstimator(mechAngleEstimator),
      mParam(param),
      mSystick(systick),
      mVelocity(0.0f),
      mRecords(),
      mMaxSamples(0),
      mMaxSampleWindowTime(0),
      mMinSampleTime(0),
      mEnabled(false) {}

void VelocityEstimator::enable() {
  mMaxSamples = mParam->getValue<int32_t>("velocity_sample_window_size"_hash);
  mMaxSampleWindowTime =
      mParam->getValue<int32_t>("velocity_sample_window_time"_hash);
  mMinSampleTime =
      mParam->getValue<int32_t>("velocity_sample_minimal_duration"_hash);

  if (!mMechAngleEstimator->enabled()) {
    mMechAngleEstimator->enable();
  }
  mEnabled = true;

  // force sync to initialize first record
  mMechAngleEstimator->sync();
  RecordItem item{mSystick->systick_ms(),
                  mMechAngleEstimator->getMechanicalAngle()};
  mRecords.push_back(item);
  // push tow items to avoid diff time is zero
  item.timestamp += mMinSampleTime;
  mRecords.push_back(item);
}

void VelocityEstimator::disable() {
  if (mMechAngleEstimator->enabled()) {
    mMechAngleEstimator->disable();
  }
  mRecords.clear();
  mEnabled = false;
  mVelocity = 0;
}

bool VelocityEstimator::enabled() {
  return mEnabled && mMechAngleEstimator->enabled();
}

void VelocityEstimator::sync() {
  uint32_t current = mSystick->systick_ms();
  uint32_t last = mRecords.back().timestamp;
  float angleDiff, timeDiff;

  // skip if not match minimal sample time requirement
  if ((int64_t(current) - int64_t(last)) < mMinSampleTime) {
    return;
  }

  if (mMechAngleEstimator->needSync(mSyncId)) {
    mMechAngleEstimator->sync();
  }

  // item number must less than max samples
  while (mRecords.size() >= mMaxSamples) {
    mRecords.pop_front();
  }

  // drop oldest item if time window is full
  while (current - mRecords.front().timestamp > mMaxSampleWindowTime) {
    mRecords.pop_front();
  }

  mRecords.push_back({current, mMechAngleEstimator->getMechanicalAngle()});
  angleDiff = mRecords.back().angle - mRecords.front().angle;
  timeDiff = mRecords.back().timestamp - mRecords.front().timestamp;
  mVelocity = (angleDiff / 360.0f) / (timeDiff / 60000.0f);
}

void VelocityEstimator::calibrate() {}

bool VelocityEstimator::needCalibrate() { return false; }

float VelocityEstimator::getVelocity() { return mVelocity; }

}  // namespace motorctl
}  // namespace coriander
