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
#include "coriander/os/isystick.h"
#include "coriander/parameters.h"

namespace coriander {
namespace motorctl {

/**
 * @brief Motor control for calibration
 *
 * @param motor_supply_voltage required float Motor supply voltage
 * @param calibrate_voltage required float Voltage for calibration
 * @param calibrate_duration required int32_t required Duration for calibration
 */
struct MotorCtlCalibrate : public IMotorCtl {
  using ISystick = os::ISystick;
  MotorCtlCalibrate(std::shared_ptr<IBldcDriver> motor,
                    std::shared_ptr<IElecAngleEstimator> elecAngleEstimator,
                    std::shared_ptr<ParameterBase> param,
                    std::shared_ptr<IBoardEvent> boardEvent,
                    std::shared_ptr<ISystick> systick);
  virtual void start();
  virtual void stop();
  virtual void loop();

  virtual void emergencyStop();
  virtual bool fatalError();

 private:
  std::shared_ptr<IBldcDriver> mMotor;
  std::shared_ptr<IElecAngleEstimator> mElecAngleEstimator;
  std::shared_ptr<ParameterBase> mParam;
  std::shared_ptr<IBoardEvent> mBoardEvent;
  std::shared_ptr<ISystick> mSystick;

  float mMotorSupplyVoltage;
  float mCalibrateVoltage;
  int32_t mCalibrateDuration;
  uint32_t startTimestamp;
  unsigned mSampleId;
};
}  // namespace motorctl
}  // namespace coriander
