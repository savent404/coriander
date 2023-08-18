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
// clang-format off
BETTER_ENUM(ParamId, int,
  MotorCtlMode,
  PolePair,
  ElecAngleOffset,
  PersistRawElecAngle,
  PersistRawMechAngle,
  MechAngleOffset,
  CalibrateVoltage,
  CalibrateDuration,
  MotorSupplyVoltage,
  MechAnglePidP,
  MechAnglePidI,
  MechAnglePidD,
  MechAnglePidOutputRamp,
  MechAnglePidLimit,
  TargetPosition,
  TargetVelocity,
  VelocityPidP,
  VelocityPidI,
  VelocityPidD,
  VelocityPidOutputRamp,
  VelocityPidLimit,
  VelocitySampleWindowSize,
  VelocitySampleWindowTime,
  VelocitySampleMinimalDuration,
  Unknow, MAX_PARAM_ID);
// clang-format on

struct ParamDescriber {
  static inline constexpr const char *getParamDescription(ParamId id) {
    const char *desc[] = {
        [ParamId::MotorCtlMode] = "0:dummy, 1:velocity, 2:position",
        [ParamId::PolePair] = "motor pole pair",
        [ParamId::ElecAngleOffset] = "electrical angle offset, unit: degree",
        [ParamId::PersistRawElecAngle] = "electrical angle, "
                                         "persistent value, unit: degree",
        [ParamId::PersistRawMechAngle] = "mechanical angle, "
                                         "persistent value. unit: degree",
        [ParamId::MechAngleOffset] = "mechanical angle offset, unit: degree",
        [ParamId::CalibrateVoltage] = "calibrate voltage, unit: volt",
        [ParamId::CalibrateDuration] = "calibrate duration, unit: ms",
        [ParamId::MotorSupplyVoltage] = "motor supply voltage, unit: volt",
        [ParamId::MechAnglePidP] = "",
        [ParamId::MechAnglePidI] = "",
        [ParamId::MechAnglePidD] = "",
        [ParamId::MechAnglePidOutputRamp] = "maxium ramp of output",
        [ParamId::MechAnglePidLimit] = "maxium output",
        [ParamId::TargetPosition] = "target position, unit: degree",
        [ParamId::TargetVelocity] = "target velocity, unit: RPM",
        [ParamId::VelocityPidP] = "",
        [ParamId::VelocityPidI] = "",
        [ParamId::VelocityPidD] = "",
        [ParamId::VelocityPidOutputRamp] = "maxium ramp of output",
        [ParamId::VelocityPidLimit] = "maxium output",
        [ParamId::VelocitySampleWindowSize] = "window size of velocity "
                                              "estimator, default: 16",
        [ParamId::VelocitySampleWindowTime] =
            "max window time of velocity estimator, default: 1000ms",
        [ParamId::VelocitySampleMinimalDuration] =
            "minimal duration of velocity estimator, default: 10ms",
    };

    return desc[id];
  }
};

}  // namespace base
}  // namespace coriander
