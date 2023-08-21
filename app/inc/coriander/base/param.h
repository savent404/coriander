/**
 * @file param.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-15
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once

#include <array>

#include "enum.h"  // NOLINT

namespace coriander {

namespace base {

/**
 * @brief Naming code of parameters
 *
 * @details
 *  Name format:
 *  <module> module name of function
 *  <submodule> sub-module name of module
 *  <param_name> parameter name
 *
 *  prefix meaning:
 *  RT: runtime parameter
 *  NC: parameter not configurable in runtime, need a reboot to take effect
 *
 *  for runtime parameters:
 *   <module>_<submodule>_<param_name>_RT
 *
 *  for persistent parameters(sync to flash):
 *   <module>_<submodule>_<param_name>
 * @note the order of enum and description array are relaxed, and also can be changed
 * @warning we don't specific the type of parameter here, be careful when using a parameter
 *          which is not a float(the default type of parameter)
 */

// clang-format off
BETTER_ENUM(ParamId, int,
  Sensor_Mech_Position_RT,
  Sensor_Mech_Velocity_RT,
  Sensor_Motor_Voltage_RT,
  Sensor_Motor_Current_RT,
  Sensor_Motor_Temp_RT,
  MotorCtl_General_Mode_RT,
  MotorCtl_General_TargetPosition_RT,
  MotorCtl_General_TargetVelocity_RT,
  MotorCtl_MotorDriver_PolePair,
  MotorCtl_MotorDriver_PersistRawElecAngle,
  MotorCtl_MotorDriver_PersistRawMechAngle,
  MotorCtl_MotorDriver_SupplyVoltage,
  MotorCtl_Calibrate_CaliElecAngleOffset,
  MotorCtl_Calibrate_CaliMechAngleOffset,
  MotorCtl_Calibrate_CaliVoltage,
  MotorCtl_Calibrate_CaliDuration,
  MotorCtl_PosCtl_PidP,
  MotorCtl_PosCtl_PidI,
  MotorCtl_PosCtl_PidD,
  MotorCtl_PosCtl_PidOutputRamp,
  MotorCtl_PosCtl_PidLimit,
  MotorCtl_PosCtl_Freq,
  MotorCtl_SpeedCtl_PidP,
  MotorCtl_SpeedCtl_PidI,
  MotorCtl_SpeedCtl_PidD,
  MotorCtl_SpeedCtl_PidOutputRamp,
  MotorCtl_SpeedCtl_PidLimit,
  MotorCtl_SpeedCtl_Freq,
  MotorCtl_SpeedEstimator_WindowSize,
  MotorCtl_SpeedEstimator_MinDuration,
  MotorCtl_SpeedEstimator_SampleInterval,
  Unknow, MAX_PARAM_ID);
// clang-format on

struct ParamDescriber {
  static constexpr const char *getParamDescription(ParamId id) {
    const char *desc[] = {
        [ParamId::Sensor_Mech_Position_RT] = "current mechanical "
                                             "position, unit: degree",
        [ParamId::Sensor_Mech_Velocity_RT] = "current mechanical "
                                             "velocity, unit: RPM",
        [ParamId::Sensor_Motor_Voltage_RT] = "current motor "
                                             "voltage, unit V",
        [ParamId::Sensor_Motor_Current_RT] = "current motor "
                                             "current, uint A",
        [ParamId::Sensor_Motor_Temp_RT] = "current motor "
                                          "temp, unit degree",
        [ParamId::MotorCtl_General_Mode_RT] = "0:dummy, 1:velocity, 2:position",
        [ParamId::MotorCtl_General_TargetPosition_RT] =
            "target position, works in mode:2, unit: degree",
        [ParamId::MotorCtl_General_TargetVelocity_RT] =
            "target velocity, works in mode:1 unit: RPM",
        [ParamId::MotorCtl_MotorDriver_PolePair] = "motor pole pair",
        [ParamId::MotorCtl_MotorDriver_PersistRawElecAngle] =
            "electrical angle, persistent value, unit: degree",
        [ParamId::MotorCtl_MotorDriver_PersistRawMechAngle] =
            "mechanical angle, persistent value. unit: degree",
        [ParamId::MotorCtl_MotorDriver_SupplyVoltage] =
            "motor supply voltage, "
            "used when not exists voltage sensors. unit: volt",
        [ParamId::MotorCtl_Calibrate_CaliElecAngleOffset] =
            "calibrated electrical offset. "
            "need calibrate again if using "
            "relative sensor to estimate electrical angle, unit: "
            "degree",
        [ParamId::MotorCtl_Calibrate_CaliMechAngleOffset] =
            "calibrated mechanical angle offset. "
            "need calibrate again if using "
            "relative sensor to estimate mechanical angle, unit: degree",
        [ParamId::MotorCtl_Calibrate_CaliVoltage] =
            "motor output voltage when calibrating, unit: volt",
        [ParamId::MotorCtl_Calibrate_CaliDuration] =
            "duration of motor enabling when calibrating, unit: ms",
        [ParamId::MotorCtl_PosCtl_PidP] = "",
        [ParamId::MotorCtl_PosCtl_PidI] = "",
        [ParamId::MotorCtl_PosCtl_PidD] = "",
        [ParamId::MotorCtl_PosCtl_PidOutputRamp] = "maxium ramp of output",
        [ParamId::MotorCtl_PosCtl_PidLimit] = "maxium output",
        [ParamId::MotorCtl_PosCtl_Freq] = "frequency of position control, unit: Hz",
        [ParamId::MotorCtl_SpeedCtl_PidP] = "",
        [ParamId::MotorCtl_SpeedCtl_PidI] = "",
        [ParamId::MotorCtl_SpeedCtl_PidD] = "",
        [ParamId::MotorCtl_SpeedCtl_PidOutputRamp] = "maxium ramp of output",
        [ParamId::MotorCtl_SpeedCtl_PidLimit] = "maxium output",
        [ParamId::MotorCtl_SpeedCtl_Freq] = "frequency of speed control, unit: Hz",
        [ParamId::MotorCtl_SpeedEstimator_WindowSize] =
            "window size of velocity estimator, default: 16",
        [ParamId::MotorCtl_SpeedEstimator_MinDuration] =
            "max window time of velocity estimator, default: 1000ms",
        [ParamId::MotorCtl_SpeedEstimator_SampleInterval] =
            "minimal duration of velocity estimator, default: 10ms",
    };

    return desc[id];
  }
};

}  // namespace base
}  // namespace coriander
