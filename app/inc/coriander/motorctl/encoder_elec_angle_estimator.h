/**
 * @file encoder_elec_angle_estimator.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-03
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

#include <memory>

#include "coriander/base/const_hash.h"
#include "coriander/base/ilogger.h"
#include "coriander/motorctl/ielec_angle_estimator.h"
#include "coriander/motorctl/iencoder.h"
#include "coriander/parameter_requirements.h"
#include "coriander/parameters.h"

namespace coriander {
namespace motorctl {

/**
 * @brief Elecangle estimator based on encoder
 *
 */
struct EncoderElecAngleEstimator : public IElecAngleEstimator,
                                   public IParamReq {
  using ILogger = coriander::base::ILogger;
  EncoderElecAngleEstimator(
      std::shared_ptr<IEncoder> encoder, std::shared_ptr<Parameter> param,
      std::shared_ptr<ILogger> logger,
      std::shared_ptr<IParamReqValidator> paramReqValidator) noexcept;

  virtual void enable();
  virtual void disable();
  virtual void sync();
  virtual bool enabled();
  virtual void calibrate();
  virtual bool needCalibrate();
  virtual float getElectricalAngle() noexcept;

  virtual const ParameterRequireItem* requiredParameters() const {
    constexpr static const ParameterRequireItem items[] = {
        {"MotorCtl_MotorDriver_PolePair", TypeId::Int32},
        {"MotorCtl_MotorDriver_PersistRawElecAngle", TypeId::Float},
        {"Sensor_Encoder_ReverseElecAngle", TypeId::Int32},
        PARAMETER_REQ_EOF};

    return &items[0];
  }

 private:
  std::shared_ptr<IEncoder> mEncoder;
  std::shared_ptr<Parameter> mParam;
  std::shared_ptr<ILogger> mLogger;
  int32_t mPolePair;
  float mElecAngle;
  float mRawElecAngle;
  float mPersistOffset;
  float mElecAngleOffset;
  bool mNeedCalibrate;
  bool mReverse;
};
}  // namespace motorctl
}  // namespace coriander
