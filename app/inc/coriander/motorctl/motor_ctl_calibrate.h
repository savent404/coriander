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

#include "coriander/iboard_event.h"
#include "coriander/motorctl/ibldc_driver.h"
#include "coriander/motorctl/ielec_angle_estimator.h"
#include "coriander/motorctl/imech_angle_estimator.h"
#include "coriander/motorctl/imotorctl.h"
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
  using ISystick = os::ISystick;
  MotorCtlCalibrate(std::shared_ptr<IBldcDriver> motor,
                    std::shared_ptr<IElecAngleEstimator> elecAngleEstimator,
                    std::shared_ptr<ParameterBase> param,
                    std::shared_ptr<IBoardEvent> boardEvent,
                    std::shared_ptr<ISystick> systick,
                    std::shared_ptr<IParamReqValidator> paramReqValidator);
  virtual void start();
  virtual void stop();
  virtual void loop();

  virtual void emergencyStop();
  virtual bool fatalError();

  virtual const ParameterRequireItem* requiredParameters() const {
    using Type = coriander::base::TypeId;
    static const ParameterRequireItem items[] = {
        {"CalibrateVoltage", Type::Float},
        {"CalibrateDuration", Type::Int32},
        {"MotorSupplyVoltage", Type::Float},
        PARAMETER_REQ_EOF};
    return items;
  }

 private:
  std::shared_ptr<IBldcDriver> mMotor;
  std::shared_ptr<IElecAngleEstimator> mElecAngleEstimator;
  std::shared_ptr<ParameterBase> mParam;
  std::shared_ptr<IBoardEvent> mBoardEvent;
  std::shared_ptr<ISystick> mSystick;

  // parameters
  float mCalibrateVoltage;
  int32_t mCalibrateDuration;
  float mMotorSupplyVoltage;

  // runtime variables
  SensorHandler mSensorHandler;
  uint32_t startTimestamp;
};
}  // namespace motorctl
}  // namespace coriander
