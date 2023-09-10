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

#include "coriander/base/loggerstream.h"
#include "coriander/base/math.h"

namespace {
static float modular_angle(float angle) {
  angle = coriander::base::math::fmodf(angle, 360.0f);
  if (angle < 0) {
    angle += 360.0f;
  }
  return angle;
}
static inline float reverse_angle(float angle) {
  angle = modular_angle(-angle);
  return angle;
}
}  // namespace

namespace coriander {
namespace motorctl {

EncoderElecAngleEstimator::EncoderElecAngleEstimator(
    std::shared_ptr<IEncoder> encoder, std::shared_ptr<Parameter> param,
    std::shared_ptr<ILogger> logger,
    std::shared_ptr<IParamReqValidator> paramReqValidator) noexcept
    : mEncoder(encoder),
      mParam(param),
      mPolePair(-1),
      mLogger(logger),
      mElecAngle(0),
      mRawElecAngle(0),
      mPersistOffset(0),
      mElecAngleOffset(-1),
      mNeedCalibrate(true) {
  paramReqValidator->addParamReq(this);
}

void EncoderElecAngleEstimator::enable() {
  // try to get parameter from parameter server
  mPolePair = mParam->getValue<int32_t>(ParamId::MotorCtl_MotorDriver_PolePair);
  if (mParam->has(ParamId::MotorCtl_Calibrate_CaliElecAngleOffset)) {
    mElecAngleOffset = mParam->getValue<float>(
        ParamId::MotorCtl_Calibrate_CaliElecAngleOffset);
    mNeedCalibrate = false;
  }

  if (!mEncoder->enabled()) {
    mEncoder->enable();
  }

  if (mParam->has(ParamId::MotorCtl_MotorDriver_PersistRawElecAngle)) {
    // force sync encoder and initialize mRawElecAngle
    mEncoder->sync();
    mPersistOffset = 0;
    getElectricalAngle();

    mPersistOffset =
        mRawElecAngle - mParam->getValue<float>(
                            ParamId::MotorCtl_MotorDriver_PersistRawElecAngle);
  }

  mReverse =
      mParam->getValue<int32_t>(ParamId::Sensor_Encoder_ReverseElecAngle) != 0;
}

void EncoderElecAngleEstimator::disable() {
  if (mEncoder->enabled()) {
    mEncoder->disable();
  }
  if (mParam->has(ParamId::MotorCtl_MotorDriver_PersistRawElecAngle)) {
    mParam->setValue(ParamId::MotorCtl_MotorDriver_PersistRawElecAngle,
                     mRawElecAngle);
  }
}

void EncoderElecAngleEstimator::sync() {
  if (mEncoder->needSync(mSyncId)) {
    mEncoder->sync();
  }
}

bool EncoderElecAngleEstimator::enabled() { return mEncoder->enabled(); }

void EncoderElecAngleEstimator::calibrate() {
  using Property = base::Property;
  coriander::base::LoggerStream stream(mLogger);
  float t = static_cast<float>(mEncoder->getEncoderCount()) /
            mEncoder->getEncoderCountPerRound() * 360.0f * mPolePair;
  if (mReverse) {
    t = reverse_angle(t);
  }
  mRawElecAngle = modular_angle(t - mPersistOffset);
  mElecAngleOffset = -mRawElecAngle;
  if (mParam->has(ParamId::MotorCtl_Calibrate_CaliElecAngleOffset)) {
    mParam->setValue(ParamId::MotorCtl_Calibrate_CaliElecAngleOffset,
                     mElecAngleOffset);
  } else {
    mParam->add(Property{mElecAngleOffset,
                         ParamId::MotorCtl_Calibrate_CaliElecAngleOffset});
  }
  mNeedCalibrate = false;

  stream << "Calibrated electrical angle offset: " << mElecAngleOffset
         << std::endl;
}

bool EncoderElecAngleEstimator::needCalibrate() { return mNeedCalibrate; }

float EncoderElecAngleEstimator::getElectricalAngle() noexcept {
  float t = static_cast<float>(mEncoder->getEncoderCount()) /
            mEncoder->getEncoderCountPerRound() * 360.0f * mPolePair;
  if (mReverse) {
    t = reverse_angle(t);
  }
  mRawElecAngle = modular_angle(t - mPersistOffset);
  mElecAngle = modular_angle(mRawElecAngle + mElecAngleOffset);
  return mElecAngle;
}

}  // namespace motorctl
}  // namespace coriander
