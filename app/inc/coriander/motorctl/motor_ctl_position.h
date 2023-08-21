/**
 * @file motor_ctl_position.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-08
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once

#include <initializer_list>
#include <memory>
#include <utility>

#include "coriander/motorctl/duration_estimator.h"
#include "coriander/motorctl/imech_angle_estimator.h"
#include "coriander/motorctl/motor_ctl_velocity.h"
#include "coriander/motorctl/sensor_handler.h"
#include "coriander/parameter_requirements.h"

namespace coriander {
namespace motorctl {

struct MotorCtlPosition : public IMotorCtl, public IParamReq {
  using DurationTimeout = DurationExpired<DurationEstimatorUnit::US>;
  using Duration = DurationEstimator<DurationEstimatorType::OneShot,
                                     DurationEstimatorUnit::US>;
  MotorCtlPosition(std::shared_ptr<MotorCtlVelocity> motorCtlVelocity,
                   std::shared_ptr<IMechAngleEstimator> mechAngleEstimator,
                   std::unique_ptr<Duration> durationEstimator,
                   std::unique_ptr<DurationTimeout> durationTimeout,
                   std::shared_ptr<ParameterBase> parameters,
                   std::shared_ptr<IParamReqValidator> paramReqValidator)
      : mMotorCtlVelocity{motorCtlVelocity},
        mMechAngleEstimator{mechAngleEstimator},
        mParameters{parameters},
        mDurationEstimator{std::move(durationEstimator)},
        mDurationTimeout{std::move(durationTimeout)},
        mSensorHandler{mMechAngleEstimator},
        mMechAnglePid(0.0f, 0.0f, 0.0f, 0.0f, 0.0f) {
    paramReqValidator->addParamReq(this);
  }

  virtual void start();
  virtual void stop();
  virtual void loop();

  virtual void emergencyStop();
  virtual bool fatalError();

  virtual const ParameterRequireItem* requiredParameters() const {
    using Type = coriander::base::TypeId;
    static const ParameterRequireItem items[] = {
        {"MotorCtl_PosCtl_PidP", Type::Float},
        {"MotorCtl_PosCtl_PidI", Type::Float},
        {"MotorCtl_PosCtl_PidD", Type::Float},
        {"MotorCtl_PosCtl_PidOutputRamp", Type::Float},
        {"MotorCtl_PosCtl_PidLimit", Type::Float},
        {"MotorCtl_PosCtl_Freq", Type::UInt32},
        {"MotorCtl_General_TargetPosition_RT", Type::Float},
        PARAMETER_REQ_EOF};
    return items;
  }

 protected:
  std::shared_ptr<MotorCtlVelocity> mMotorCtlVelocity;
  std::shared_ptr<IMechAngleEstimator> mMechAngleEstimator;
  std::shared_ptr<ParameterBase> mParameters;
  std::unique_ptr<Duration> mDurationEstimator;
  std::unique_ptr<DurationTimeout> mDurationTimeout;

  // parameters
  float mTargetPosition;

  // runtime vars
  SensorHandler mSensorHandler;
  Pid mMechAnglePid;
};

}  // namespace motorctl
}  // namespace coriander
