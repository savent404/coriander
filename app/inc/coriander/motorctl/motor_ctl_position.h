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

#include "coriander/motorctl/duration_estimator.h"
#include "coriander/motorctl/imech_angle_estimator.h"
#include "coriander/motorctl/motor_ctl_velocity.h"
#include "coriander/motorctl/sensor_handler.h"
#include "coriander/parameter_requirements.h"

namespace coriander {
namespace motorctl {

struct MotorCtlPosition : public IMotorCtl, public IParamReq {
  MotorCtlPosition(std::shared_ptr<MotorCtlVelocity> motorCtlVelocity,
                   std::shared_ptr<IMechAngleEstimator> mechAngleEstimator,
                   std::unique_ptr<DurationEstimatorDefault> durationEstimator,
                   std::shared_ptr<ParameterBase> parameters,
                   std::shared_ptr<base::ILogger> logger,
                   std::shared_ptr<IParamReqValidator> paramReqValidator)
      : mMotorCtlVelocity{motorCtlVelocity},
        mMechAngleEstimator{mechAngleEstimator},
        mParameters{parameters},
        mDurationEstimator{std::move(durationEstimator)},
        mLogger{logger},
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
        {"mech_angle_pid_p", Type::Float},
        {"mech_angle_pid_i", Type::Float},
        {"mech_angle_pid_d", Type::Float},
        {"mech_angle_pid_output_ramp", Type::Float},
        {"mech_angle_pid_limit", Type::Float},
        {"target_position", Type::Float},
        PARAMETER_REQ_EOF};
    return items;
  }

 protected:
  std::shared_ptr<MotorCtlVelocity> mMotorCtlVelocity;
  std::shared_ptr<IMechAngleEstimator> mMechAngleEstimator;
  std::shared_ptr<ParameterBase> mParameters;
  std::unique_ptr<DurationEstimatorDefault> mDurationEstimator;
  std::shared_ptr<base::ILogger> mLogger;

  // parameters
  float mTargetPosition;

  // runtime vars
  SensorHandler mSensorHandler;
  Pid mMechAnglePid;
};

}  // namespace motorctl
}  // namespace coriander