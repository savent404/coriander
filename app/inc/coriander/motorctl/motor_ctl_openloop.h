/**
 * @file motor_ctl_openloop.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-09-02
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once

#include <memory>
#include <utility>

#include "coriander/motorctl/duration_estimator.h"
#include "coriander/motorctl/foc_motor_driver.h"
#include "coriander/motorctl/ielec_angle_estimator.h"
#include "coriander/motorctl/imotorctl.h"
#include "coriander/motorctl/iphase_current_estimator.h"
#include "coriander/motorctl/ivelocity_estimator.h"
#include "coriander/motorctl/sensor_handler.h"
#include "coriander/parameter_requirements.h"
#include "coriander/parameter_requirements_validator.h"
#include "coriander/parameters.h"

namespace coriander::motorctl {

struct MotorCtlOpenLoop : public IMotorCtl, coriander::IParamReq {
  using DurationExpired =
      detail::DurationExpired<detail::DurationEstimatorUnit::MS>;

  explicit MotorCtlOpenLoop(
      std::shared_ptr<FocMotorDriver> motor,
      std::shared_ptr<Parameter> parameters,
      std::unique_ptr<DurationExpired> durationEstimator,
      std::shared_ptr<IPhaseCurrentEstimator> phaseCurrentEstimator,
      std::shared_ptr<IElecAngleEstimator> elecAngleEstimator,
      std::shared_ptr<IVelocityEstimator> velocityEstimator,
      std::shared_ptr<IParamReqValidator> paramReqValidator)
      : mFocMotorDriver(motor),
        mParameters(parameters),
        mDurationTimeout(std::move(durationEstimator)),
        mPhaseCurrentEstimator(phaseCurrentEstimator),
        mElecAngleEstimator(elecAngleEstimator),
        mVelocityEstimator(velocityEstimator),
        mSensorHandler{phaseCurrentEstimator, elecAngleEstimator,
                       velocityEstimator} {
    paramReqValidator->addParamReq(this);
  }
  virtual ~MotorCtlOpenLoop() = default;

  virtual void start();
  virtual void stop();
  virtual void loop();

  virtual void emergencyStop();
  virtual bool fatalError();

  virtual const ParameterRequireItem* requiredParameters() const {
    using Type = coriander::base::TypeId;
    static const ParameterRequireItem items[] = {
        {"MotorCtl_MotorDriver_SupplyVoltage", Type::Float},
        {"MotorCtl_OpenLoop_OutVoltage", Type::Float},
        {"MotorCtl_OpenLoop_UseElecAngle", Type::Int32},
        PARAMETER_REQ_EOF};
    return items;
  }

 private:
  std::shared_ptr<FocMotorDriver> mFocMotorDriver;
  std::shared_ptr<Parameter> mParameters;
  std::unique_ptr<DurationExpired> mDurationTimeout;
  std::shared_ptr<IPhaseCurrentEstimator> mPhaseCurrentEstimator;
  std::shared_ptr<IElecAngleEstimator> mElecAngleEstimator;
  std::shared_ptr<IVelocityEstimator> mVelocityEstimator;

  SensorHandler mSensorHandler;

  float mDutyCycleUq = 0;
  float mCurrentAngle = 0;
  bool mUseElecAngle = 0;
};
}  // namespace coriander::motorctl
