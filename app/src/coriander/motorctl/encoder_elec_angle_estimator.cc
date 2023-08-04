/**
 * @file encoder_elec_angle_estimator.cc
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-03
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "coriander/motorctl/encoder_elec_angle_estimator.h"

#include <cmath>

namespace coriander {
namespace motorctl {

EncoderElecAngleEstimator::EncoderElecAngleEstimator(
    std::shared_ptr<IEncoder> encoder,
    std::shared_ptr<ParameterBase> param) noexcept
    : mEncoder(encoder),
      mParam(param),
      mPolePair(-1),
      mElecAngle(0),
      mRawElecAngle(0),
      mPersistOffset(0),
      mElecAngleOffset(-1),
      mNeedCalibrate(true) {}

void EncoderElecAngleEstimator::enable() {
  // try to get parameter from parameter server
  mPolePair = mParam->getValue<int>("pole_pair"_hash);
  if (mParam->has("elec_angle_offset"_hash)) {
    mElecAngleOffset = mParam->getValue<float>("elec_angle_offset"_hash);
    mNeedCalibrate = false;
  }

  if (!mEncoder->enabled()) {
    mEncoder->enable();
  }

  if (mParam->has("persist_raw_elec_angle"_hash)) {
    // force sync encoder and initialize mRawElecAngle
    mEncoder->sync();
    mPersistOffset = 0;
    getElectricalAngle();

    mPersistOffset =
        mRawElecAngle - mParam->getValue<float>("persist_raw_elec_angle"_hash);
  }
}

void EncoderElecAngleEstimator::disable() {
  if (mEncoder->enabled()) {
    mEncoder->disable();
  }
  if (mParam->has("persist_raw_elec_angle"_hash)) {
    mParam->setValue("persist_raw_elec_angle"_hash, mRawElecAngle);
  }
}

void EncoderElecAngleEstimator::sync() {
  if (mEncoder->needSync(mSyncId)) {
    mEncoder->sync();
  }
}

bool EncoderElecAngleEstimator::enabled() { return mEncoder->enabled(); }

void EncoderElecAngleEstimator::calibrate() {
  mNeedCalibrate = false;
  mElecAngleOffset = -mRawElecAngle;
  if (mParam->has("elec_angle_offset"_hash)) {
    mParam->setValue("elec_angle_offset"_hash, mElecAngleOffset);
  }
}

bool EncoderElecAngleEstimator::needCalibrate() { return mNeedCalibrate; }

static float modular_angle(float angle) {
  angle = std::fmodf(angle, 360.0f);
  if (angle < 0) {
    angle += 360.0f;
  }
  return angle;
}

float EncoderElecAngleEstimator::getElectricalAngle() noexcept {
  float t = float(mEncoder->getEncoderCount()) /
            mEncoder->getEncoderCountPerRound() * 360.0f * mPolePair;
  mRawElecAngle = modular_angle(t - mPersistOffset);
  mElecAngle = modular_angle(mRawElecAngle + mElecAngleOffset);
  return mElecAngle;
}

}  // namespace motorctl
}  // namespace coriander
