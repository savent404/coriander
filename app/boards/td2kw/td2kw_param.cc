/**
 * @file td2kw_param.cc
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-09-12
 *
 * Copyright 2023 savent_gate
 *
 */
#include <memory>

#include "coriander/parameters.h"

void board_parameter_default_value_setup(
    std::shared_ptr<coriander::Parameter> param) {
  using Property = coriander::base::Property;
  using ID = coriander::base::ParamId;
  using P = Property;

  static const Property properties[] = {
      P{0, ID::MotorCtl_General_Mode_RT},
      P{4, ID::MotorCtl_MotorDriver_PolePair},
      P{0.0f, ID::MotorCtl_MotorDriver_PersistRawElecAngle},
      P{0.0f, ID::MotorCtl_MotorDriver_PersistRawMechAngle},
      P{0.0f, ID::MotorCtl_Calibrate_CaliMechAngleOffset},
      P{3.0f, ID::MotorCtl_Calibrate_CaliVoltage},
      P{3000, ID::MotorCtl_Calibrate_CaliDuration},
      P{24.0f, ID::MotorCtl_MotorDriver_SupplyVoltage},
      P{0.1f, ID::MotorCtl_PosCtl_PidP},
      P{0.0f, ID::MotorCtl_PosCtl_PidI},
      P{0.0f, ID::MotorCtl_PosCtl_PidD},
      P{100.0f, ID::MotorCtl_PosCtl_PidOutputRamp},
      P{960.0f, ID::MotorCtl_PosCtl_PidLimit},
      P{100, ID::MotorCtl_PosCtl_Freq},
      P{3600.0f, ID::MotorCtl_General_TargetPosition_RT},
      P{0.0f, ID::MotorCtl_General_TargetVelocity_RT},
      P{0.01f, ID::MotorCtl_SpeedCtl_PidP},
      P{0.002f, ID::MotorCtl_SpeedCtl_PidI},
      P{0.0f, ID::MotorCtl_SpeedCtl_PidD},
      P{100.0f, ID::MotorCtl_SpeedCtl_PidOutputRamp},
      P{10.0f, ID::MotorCtl_SpeedCtl_PidLimit},
      P{500, ID::MotorCtl_SpeedCtl_Freq},
      P{16, ID::MotorCtl_SpeedEstimator_WindowSize},
      P{1000, ID::MotorCtl_SpeedEstimator_MinDuration},
      P{10, ID::MotorCtl_SpeedEstimator_SampleInterval},
      P{0, ID::Sensor_Mech_Position_RT},
      P{0, ID::Sensor_Mech_Velocity_RT},
      P{0, ID::Sensor_Motor_Current_RT},
      P{0, ID::Sensor_Motor_Temp_RT},
      P{0, ID::Sensor_Motor_Voltage_RT},
      P{5e-3f, ID::MotorCtl_CurrCtl_Lpf_TimeConstant},    // LPF: 200Hz
      P{10e-3f, ID::MotorCtl_SpeedCtl_Lpf_TimeConstant},  // 100Hz
      P{100e-3f, ID::MotorCtl_PosCtl_Lpf_TimeConstant},   // 10Hz
      P(2.0f, ID::MotorCtl_OpenLoop_OutVoltage),          // 2.0V
      P{0, ID::MotorCtl_OpenLoop_UseElecAngle},
      P{0.1f, ID::MotorCtl_CurrCtl_PidP},
      P{0.0f, ID::MotorCtl_CurrCtl_PidI},
      P{0.0f, ID::MotorCtl_CurrCtl_PidD},
      P(0.3f, ID::MotorCtl_CurrCtl_PidOutputRamp),
      P{2.5f, ID::MotorCtl_CurrCtl_PidLimit},
      P{4000, ID::MotorCtl_CurrCtl_Freq},
      P{0.0f, ID::MotorCtl_General_TargetCurrentD_RT},
      P{0.0f, ID::MotorCtl_General_TargetCurrentQ_RT},
      P{0, ID::Sensor_Encoder_ReverseElecAngle},
  };
  for (auto& p : properties) {
    param->add(p);
  }
}
