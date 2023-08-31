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
#include <memory>

#include "coriander/motorctl/imech_angle_estimator.h"
#include "coriander/motorctl/ivelocity_estimator.h"
#include "coriander/os/isystick.h"
#include "coriander/parameter_requirements.h"
#include "coriander/parameters.h"

namespace coriander {
namespace motorctl {

/**
 * @brief Velocity Estimator based on time windows(default method)
 *
 */
struct VelocityEstimator : public IVelocityEstimator, public IParamReq {
  using ISystick = coriander::os::ISystick;

  struct RecordItem {
    uint32_t timestamp;
    float angle;
  };

  using Records = std::list<RecordItem>;

  explicit VelocityEstimator(
      std::shared_ptr<IMechAngleEstimator> mechAngleEstimator,
      std::shared_ptr<Parameter> param, std::shared_ptr<ISystick> systick,
      std::shared_ptr<IParamReqValidator> paramReqValidator);
  virtual void enable();
  virtual void disable();
  virtual bool enabled();
  virtual void sync();
  virtual void calibrate();
  virtual bool needCalibrate();
  virtual float getVelocity();

  virtual const ParameterRequireItem* requiredParameters() const {
    using coriander::base::operator""_hash;
    using Type = coriander::base::TypeId;
    static const ParameterRequireItem requiredParam[] = {
        {"MotorCtl_SpeedEstimator_WindowSize", Type::Int32},
        {"MotorCtl_SpeedEstimator_MinDuration", Type::Int32},
        {"MotorCtl_SpeedEstimator_SampleInterval", Type::Int32},
        PARAMETER_REQ_EOF};
    return requiredParam;
  }

 private:
  std::shared_ptr<IMechAngleEstimator> mMechAngleEstimator;
  std::shared_ptr<Parameter> mParam;
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
