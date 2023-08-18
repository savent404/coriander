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

#include "coriander/base/ilogger.h"
#include "coriander/motorctl/duration_estimator.h"
#include "coriander/motorctl/foc_motor_driver.h"
#include "coriander/motorctl/ielec_angle_estimator.h"
#include "coriander/motorctl/imotorctl.h"
#include "coriander/motorctl/ivelocity_estimator.h"
#include "coriander/motorctl/pid.h"
#include "coriander/motorctl/sensor_handler.h"
#include "coriander/os/isystick.h"
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
  using ISystick = os::ISystick;
  using ILogger = base::ILogger;
  MotorCtlVelocity(std::shared_ptr<IElecAngleEstimator> elecAngleEstimator,
                   std::shared_ptr<IVelocityEstimator> velocityEstimator,
                   std::shared_ptr<ParameterBase> parameters,
                   std::shared_ptr<FocMotorDriver> focMotorDriver,
                   std::unique_ptr<DurationEstimator> durationEstimator,
                   std::shared_ptr<base::ILogger> logger,
                   std::shared_ptr<IParamReqValidator> paramReqValidator)
      : mElecAngleEstimator{elecAngleEstimator},
        mVelocityEstimator{velocityEstimator},
        mParameters{parameters},
        mFocMotorDriver{focMotorDriver},
        mDurationEstimator{std::move(durationEstimator)},
        mLogger{logger},
        mSensorHandler{mElecAngleEstimator, mVelocityEstimator},
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
        {"TargetVelocity", Type::Float},
        {"MotorSupplyVoltage", Type::Float},
        {"VelocityPidP", Type::Float},
        {"VelocityPidI", Type::Float},
        {"VelocityPidD", Type::Float},
        {"VelocityPidOutputRamp", Type::Float},
        {"VelocityPidLimit", Type::Float},
        PARAMETER_REQ_EOF};
    return items;
  }

 private:
  // dependencies
  std::shared_ptr<IElecAngleEstimator> mElecAngleEstimator;
  std::shared_ptr<IVelocityEstimator> mVelocityEstimator;
  std::shared_ptr<ParameterBase> mParameters;
  std::shared_ptr<FocMotorDriver> mFocMotorDriver;
  std::unique_ptr<DurationEstimator> mDurationEstimator;
  std::shared_ptr<ILogger> mLogger;

  // parameter
  float mTargetVelocity;
  float mMotorSupplyVoltage;

  // runtime vars
  SensorHandler mSensorHandler;
  Pid mVelocityPid;
};

}  // namespace motorctl
}  // namespace coriander
