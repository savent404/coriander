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

#include "coriander/base/ilogger.h"
#include "coriander/motorctl/duration_estimator.h"
#include "coriander/motorctl/foc_motor_driver.h"
#include "coriander/motorctl/ielec_angle_estimator.h"
#include "coriander/motorctl/imotorctl.h"
#include "coriander/motorctl/ivelocity_estimator.h"
#include "coriander/motorctl/pid.h"
#include "coriander/motorctl/sensor_handler.h"
#include "coriander/os/isystick.h"
#include "coriander/parameters.h"

namespace coriander {
namespace motorctl {

/**
 * @brief Motor control for velocity control(without current loop)
 *
 * @param target_velocity required float Target velocity
 * @param motor_supply_voltage required float Motor supply voltage
 * @param velocity_pid_p required float P of velocity pid
 * @param velocity_pid_i required float I of velocity pid
 * @param velocity_pid_d required float D of velocity pid
 * @param velocity_pid_output_ramp required float Output ramp of velocity pid
 * @param velocity_pid_limit required float Limit of velocity pid
 */
struct MotorCtlVelocity : public IMotorCtl {
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
                   std::shared_ptr<base::ILogger> logger)
      : mElecAngleEstimator{elecAngleEstimator},
        mVelocityEstimator{velocityEstimator},
        mParameters{parameters},
        mFocMotorDriver{focMotorDriver},
        mSensorHandler{mElecAngleEstimator, mVelocityEstimator},
        mVelocityPid{0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
        mDurationEstimator{std::move(durationEstimator)},
        mLogger{logger} {}

  virtual void start();
  virtual void stop();
  virtual void loop();

  virtual void emergencyStop();
  virtual bool fatalError();

 protected:
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
