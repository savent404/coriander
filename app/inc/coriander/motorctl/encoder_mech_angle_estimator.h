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

#include <memory>

#include "coriander/base/const_hash.h"
#include "coriander/motorctl/iencoder.h"
#include "coriander/motorctl/imech_angle_estimator.h"
#include "coriander/parameter_requirements.h"
#include "coriander/parameters.h"

namespace coriander {
namespace motorctl {

/**
 * @brief Mechanical angle estimator based on encoder
 */
struct EncoderMechAngleEstimator : public IMechAngleEstimator,
                                   public IParamReq {
  EncoderMechAngleEstimator(
      std::shared_ptr<IEncoder> encoder, std::shared_ptr<ParameterBase> param,
      std::shared_ptr<IParamReqValidator> paramReqValidator) noexcept;

  virtual void enable();
  virtual void disable();
  virtual bool enabled();
  virtual void sync();
  virtual void calibrate();
  virtual bool needCalibrate();
  virtual float getMechanicalAngle() noexcept;

  virtual const ParameterRequireItem* requiredParameters() const {
    using coriander::base::operator""_hash;
    using Type = coriander::base::TypeId;
    static const ParameterRequireItem requiredParam[] = {
        {"MechAngleOffset", Type::Float},
        {"PersistRawMechAngle", Type::Float},
        PARAMETER_REQ_EOF};
    return requiredParam;
  }

 private:
  std::shared_ptr<IEncoder> mEncoder;
  std::shared_ptr<ParameterBase> mParam;
  float mRawMechAngle;
  float mPersistOffset;
  float mMechAngleOffset;
  float mMechAngle;
  float mNeedCalibrate;
};
}  // namespace motorctl
}  // namespace coriander
