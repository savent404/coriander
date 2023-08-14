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

#include "coriander/base/const_hash.h"
#include "coriander/motorctl/ielec_angle_estimator.h"
#include "coriander/motorctl/iencoder.h"
#include "coriander/parameter_requirements.h"
#include "coriander/parameters.h"

namespace coriander {
namespace motorctl {

using namespace coriander::base;

/**
 * @brief Elecangle estimator based on encoder
 *
 * @param pole_pair<required>     int32_t motor pole pair
 * @param elec_angle_offset       float   electrical angle offset
 * @param persist_raw_elec_angle  float   persist electrical angle, used to
 *                                        after power off
 *
 */
struct EncoderElecAngleEstimator : public IElecAngleEstimator,
                                   public IParamReq {
  EncoderElecAngleEstimator(std::shared_ptr<IEncoder> encoder,
                            std::shared_ptr<ParameterBase> param) noexcept;

  virtual void enable();
  virtual void disable();
  virtual void sync();
  virtual bool enabled();
  virtual void calibrate();
  virtual bool needCalibrate();
  virtual float getElectricalAngle() noexcept;

 protected:
  virtual const ParameterRequireItem* requiredParameters() const {
    using coriander::base::operator""_hash;
    constexpr static const ParameterRequireItem items[] = {
        {"pole_pair", "pole_pair"_hash, TypeId::Int32},
        {"elec_angle_offset", "elec_angle_offset"_hash, TypeId::Float},
        {"persist_raw_elec_angle", "persist_raw_elec_angle"_hash,
         TypeId::Float},
        {"null", 0, TypeId::Invalid}};

    return &items[0];
  }

 private:
  std::shared_ptr<IEncoder> mEncoder;
  std::shared_ptr<ParameterBase> mParam;
  int32_t mPolePair;
  float mElecAngle;
  float mRawElecAngle;
  float mPersistOffset;
  float mElecAngleOffset;
  bool mNeedCalibrate;
};
}  // namespace motorctl
}  // namespace coriander