/**
 * @file motor_ctl_current.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-21
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once

#include <memory>
#include <utility>

#include "coriander/motorctl/duration_estimator.h"
#include "coriander/motorctl/foc_motor_driver.h"
#include "coriander/motorctl/imotorctl.h"
#include "coriander/motorctl/iphase_current_estimator.h"
#include "coriander/motorctl/pid.h"
#include "coriander/motorctl/low_pass_filter.h"
#include "coriander/parameter_requirements.h"

namespace coriander {
namespace motorctl {
struct MotorCtlCurrent : public IMotorCtl, public IParamReq {
  using DurationEstimator =
      detail::DurationEstimator<detail::DurationEstimatorType::OneShot,
                                detail::DurationEstimatorUnit::US>;
  using DurationTimeout =
      detail::DurationExpired<detail::DurationEstimatorUnit::US>;

  explicit MotorCtlCurrent(
      std::shared_ptr<IPhaseCurrentEstimator> phaseCurrentEstimator,
      std::shared_ptr<ParameterBase> parameters,
      std::shared_ptr<FocMotorDriver> focMotorDriver,
      std::unique_ptr<DurationEstimator> durationEstimator,
      std::unique_ptr<DurationTimeout> durationTimeout,
      std::shared_ptr<IParamReqValidator> paramReqValidator)
      : mPhaseCurrentEstimator{phaseCurrentEstimator},
        mParams{parameters},
        mFocMotorDriver{focMotorDriver},
        mDurationEstimator{std::move(durationEstimator)},
        mDurationTimeout{std::move(durationTimeout)},
        mTargetId{0.0f},
        mTargetIq{0.0f},
        mPidD{0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
        mPidQ{0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
        mIdLpf(0.0f), mIqLpf(0.0f) {}
  virtual void start();
  virtual void stop();
  virtual void loop();

  virtual void emergencyStop();
  virtual bool fatalError();

  virtual void setTargetCurrent(float Iq, float Id);

  virtual const ParameterRequireItem* requiredParameters() const {
    using Type = coriander::base::TypeId;
    static const ParameterRequireItem items[] = {
        {"MotorCtl_CurrCtl_PidP", Type::Float},
        {"MotorCtl_CurrCtl_PidI", Type::Float},
        {"MotorCtl_CurrCtl_PidD", Type::Float},
        {"MotorCtl_CurrCtl_PidOutputRamp", Type::Float},
        {"MotorCtl_CurrCtl_PidLimit", Type::Float},
        {"MotorCtl_CurrCtl_Freq", Type::UInt32},
    };
    return items;
  }

 private:
  std::shared_ptr<IPhaseCurrentEstimator> mPhaseCurrentEstimator;
  std::shared_ptr<ParameterBase> mParams;
  std::shared_ptr<FocMotorDriver> mFocMotorDriver;
  std::shared_ptr<DurationEstimator> mDurationEstimator;
  std::shared_ptr<DurationTimeout> mDurationTimeout;

  // parameters
  float mTargetId;
  float mTargetIq;

  // runtime vars
  Pid mPidD;
  Pid mPidQ;
  LowPassFilter mIdLpf;
  LowPassFilter mIqLpf;
};  // namespace motorctl
}  // namespace motorctl
}  // namespace coriander
