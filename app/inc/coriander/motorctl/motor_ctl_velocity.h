/**
 * @file motor_ctl_velocity.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-06
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once

#include <memory>
#include <utility>

#include "coriander/motorctl/duration_estimator.h"
#include "coriander/motorctl/imotorctl.h"
#include "coriander/motorctl/ivelocity_estimator.h"
#include "coriander/motorctl/motor_ctl_current.h"
#include "coriander/motorctl/pid.h"
#include "coriander/motorctl/sensor_handler.h"
#include "coriander/parameter_requirements.h"
#include "coriander/parameters.h"

namespace coriander {
namespace motorctl {

/**
 * @brief Motor control for velocity control(without current loop)
 *
 */
struct MotorCtlVelocity : public IMotorCtl, public IParamReq {
  using DurationEstimator =
      detail::DurationEstimator<detail::DurationEstimatorType::OneShot,
                                detail::DurationEstimatorUnit::US>;
  using DurationTimeout =
      detail::DurationExpired<detail::DurationEstimatorUnit::US>;
  MotorCtlVelocity(std::shared_ptr<IVelocityEstimator> velocityEstimator,
                   std::shared_ptr<ParameterBase> parameters,
                   std::unique_ptr<DurationEstimator> durationEstimator,
                   std::unique_ptr<DurationTimeout> durationTimeout,
                   std::shared_ptr<MotorCtlCurrent> motorCtlCurrent,
                   std::shared_ptr<IParamReqValidator> paramReqValidator)
      : mVelocityEstimator{velocityEstimator},
        mParameters{parameters},
        mDurationEstimator{std::move(durationEstimator)},
        mDurationTimeout{std::move(durationTimeout)},
        mMotorCtlCurrent{motorCtlCurrent},
        mSensorHandler{mVelocityEstimator},
        mVelocityPid{0.0f, 0.0f, 0.0f, 0.0f, 0.0f} {
    paramReqValidator->addParamReq(this);
  }

  virtual void start();
  virtual void stop();
  virtual void loop();

  virtual void emergencyStop();
  virtual bool fatalError();

  void setTargetVelocity(float velocityInRpm);

  virtual const ParameterRequireItem* requiredParameters() const {
    using Type = coriander::base::TypeId;
    static const ParameterRequireItem items[] = {
        {"MotorCtl_General_TargetVelocity_RT", Type::Float},
        {"MotorCtl_MotorDriver_SupplyVoltage", Type::Float},
        {"MotorCtl_SpeedCtl_PidP", Type::Float},
        {"MotorCtl_SpeedCtl_PidI", Type::Float},
        {"MotorCtl_SpeedCtl_PidD", Type::Float},
        {"MotorCtl_SpeedCtl_PidOutputRamp", Type::Float},
        {"MotorCtl_SpeedCtl_PidLimit", Type::Float},
        {"MotorCtl_SpeedCtl_Freq", Type::UInt32},
        PARAMETER_REQ_EOF};
    return items;
  }

 private:
  // dependencies
  std::shared_ptr<MotorCtlCurrent> mMotorCtlCurrent;
  std::shared_ptr<IVelocityEstimator> mVelocityEstimator;
  std::shared_ptr<ParameterBase> mParameters;
  std::unique_ptr<DurationEstimator> mDurationEstimator;
  std::unique_ptr<DurationTimeout> mDurationTimeout;

  // parameter
  float mTargetVelocity;
  float mMotorSupplyVoltage;

  // runtime vars
  SensorHandler mSensorHandler;
  Pid mVelocityPid;
};

}  // namespace motorctl
}  // namespace coriander
