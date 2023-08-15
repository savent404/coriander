/**
 * @file mech_angle_estimator.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-03
 *
 * Copyright 2023 savent_gate
 *
 */
#include "coriander/motorctl/encoder_mech_angle_estimator.h"

namespace coriander {
namespace motorctl {
EncoderMechAngleEstimator::EncoderMechAngleEstimator(
    std::shared_ptr<IEncoder> encoder, std::shared_ptr<ParameterBase> param,
    std::shared_ptr<IParamReqValidator> paramReqValidator) noexcept
    : mEncoder(encoder),
      mParam(param),
      mRawMechAngle(0.0f),
      mPersistOffset(0.0f),
      mMechAngleOffset(0.0f),
      mMechAngle(0.0f),
      mNeedCalibrate(true) {
  paramReqValidator->addParamReq(this);
}

void EncoderMechAngleEstimator::enable() {
  if (mParam->has("MechAngleOffset"_hash)) {
    mMechAngleOffset = mParam->getValue<float>("MechAngleOffset"_hash);
    mNeedCalibrate = false;
  }

  if (!mEncoder->enabled()) {
    mEncoder->enable();
  }

  if (mParam->has("PersistRawMechAngle"_hash)) {
    // force sync encoder and initialize mRawMechAngle
    mEncoder->sync();
    mPersistOffset = 0;
    getMechanicalAngle();
    mPersistOffset =
        mRawMechAngle - mParam->getValue<float>("PersistRawMechAngle"_hash);
  }
}

void EncoderMechAngleEstimator::disable() {
  if (mEncoder->enabled()) {
    mEncoder->disable();
  }
  if (mParam->has("PersistRawMechAngle"_hash)) {
    mParam->setValue("PersistRawMechAngle"_hash, mRawMechAngle);
  }
}

bool EncoderMechAngleEstimator::enabled() { return mEncoder->enabled(); }

void EncoderMechAngleEstimator::sync() {
  if (mEncoder->needSync(mSyncId)) {
    mEncoder->sync();
  }
}

void EncoderMechAngleEstimator::calibrate() {
  mNeedCalibrate = true;
  mMechAngleOffset = -mRawMechAngle;
  if (mParam->has("MechAngleOffset"_hash)) {
    mParam->setValue("MechAngleOffset"_hash, mMechAngleOffset);
  }
}

bool EncoderMechAngleEstimator::needCalibrate() { return mNeedCalibrate; }

float EncoderMechAngleEstimator::getMechanicalAngle() noexcept {
  float t = (float(mEncoder->getEncoderCount()) /
             mEncoder->getEncoderCountPerRound());
  mRawMechAngle = (t + mEncoder->getOverflowCount()) * 360.0f - mPersistOffset;
  mMechAngle = mRawMechAngle + mMechAngleOffset;
  return mMechAngle;
}
}  // namespace motorctl
}  // namespace coriander