/**
 * @file motor_ctl_calibrate.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-04
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once

#include <memory>

#include "coriander/base/ilogger.h"
#include "coriander/iboard_event.h"
#include "coriander/motorctl/ibldc_driver.h"
#include "coriander/motorctl/ielec_angle_estimator.h"
#include "coriander/motorctl/imech_angle_estimator.h"
#include "coriander/motorctl/imotorctl.h"
#include "coriander/motorctl/iphase_current_estimator.h"
#include "coriander/motorctl/ivelocity_estimator.h"
#include "coriander/motorctl/sensor_handler.h"
#include "coriander/os/isystick.h"
#include "coriander/parameter_requirements.h"
#include "coriander/parameters.h"

namespace coriander {
namespace motorctl {

/**
 * @brief Motor control for calibration
 *
 */
struct MotorCtlCalibrate : public IMotorCtl, public IParamReq {
  enum class State {
    Calibrate_Idle,
    Calibrate_PhaseCurrent,
    Calibrate_ElecAngle,
  };

  using ISystick = os::ISystick;
  using ILogger = base::ILogger;
  MotorCtlCalibrate(
      std::shared_ptr<IBldcDriver> motor,
      std::shared_ptr<IPhaseCurrentEstimator> phaseCurrentEstimator,
      std::shared_ptr<IElecAngleEstimator> elecAngleEstimator,
      std::shared_ptr<Parameter> param, std::shared_ptr<IBoardEvent> boardEvent,
      std::shared_ptr<ISystick> systick, std::shared_ptr<ILogger> logger,
      std::shared_ptr<IParamReqValidator> paramReqValidator);
  virtual void start();
  virtual void stop();
  virtual void loop();

  virtual void emergencyStop();
  virtual bool fatalError();

  virtual const ParameterRequireItem* requiredParameters() const {
    using Type = coriander::base::TypeId;
    static const ParameterRequireItem items[] = {
        {"MotorCtl_Calibrate_CaliVoltage", Type::Float},
        {"MotorCtl_Calibrate_CaliDuration", Type::Int32},
        {"MotorCtl_MotorDriver_SupplyVoltage", Type::Float},
        PARAMETER_REQ_EOF};
    return items;
  }

 private:
  void seekCalibrateItem();
  void enterState(State state);
  void exitState();

 private:
  std::shared_ptr<IBldcDriver> mMotor;
  std::shared_ptr<IPhaseCurrentEstimator> mPhaseCurrentEstimator;
  std::shared_ptr<IElecAngleEstimator> mElecAngleEstimator;
  std::shared_ptr<Parameter> mParam;
  std::shared_ptr<IBoardEvent> mBoardEvent;
  std::shared_ptr<ISystick> mSystick;
  std::shared_ptr<ILogger> mLogger;

  // parameters
  float mCalibrateVoltage;
  int32_t mCalibrateDuration;
  float mMotorSupplyVoltage;

  // runtime variables
  SensorHandler mSensorHandler;
  uint32_t startTimestamp;
  State mState = State::Calibrate_Idle;
};
}  // namespace motorctl
}  // namespace coriander
