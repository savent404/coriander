/**
 * @file proto_td2kw_raw.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-19
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once

#include <boost/circular_buffer.hpp>
#include <cstdint>
#include <span>

namespace coriander {
namespace proto {

namespace td2kw_detail {
/**
 * @brief Parser of RxFrame
 *
 * @note RxFrame structure
 * [0] = sync1(0xEB)
 * [1] = sync2(0x90)
 * [2] = cyclic counter
 * [3] = enable
 * [4...5] = target position
 * [6...7] = (reserved)target velocity
 * [8...9] = (reserved)electrical angle offset
 * [10...11] = reserved
 * [12] = checksum
 */
struct RxFrameParser {
  RxFrameParser();

  bool parse(std::span<uint8_t> buf);

  size_t requiredBytesSize() const;

  /**
   * @brief cyclic redundancy check
   *
   * @note this function won't validate the frame
   * @return uint8_t cyclic redundancy check
   */
  uint8_t getCounter() const;

  /**
   * @brief get enable
   *
   * @note this function won't validate the frame
   * @return true enable
   * @return false disable
   */
  bool getEnable() const;

  /**
   * @brief get target position, unit: degree
   *
   * @note this function won't validate the frame
   * @return float
   */
  float getTargetPosition() const;

 private:
  bool parseOneByte(uint8_t byte);
  uint8_t getChecksum() const;
  boost::circular_buffer<uint8_t> mBuf;
};

/**
 * @brief Builder of TxFrame
 *
 * @note TxFrame structure
 * [0] = sync1(0xEB)
 * [1] = sync2(0x90)
 * [2] = cyclic counter
 * [3] = version
 * [4...5] = motor state
 * [6] = cpu state
 * [7...8] = position
 * [9...10] = velocity
 * [11...12] = current
 * [13...14] = motor temperature
 * [15...16] = motor voltage
 * [17...18] = target position
 * [19...20] target velocity(reserved)
 * [21...22] = electrical angle offset
 * [23] = checksum
 *
 * @details motor state
 * ---------------------------------------------------------------------
 *  bit 0      : motor enable, 0: disable, 1: enable
 *  bit 1...2  : motor mode 00b reverse-torque(reserved)
 *                          01b position
 *                          10b torque(reserved)
 *                          11b velocity(reserved)
 *  bit 3      : current state, 0: normal, 1: abnormal
 *  bit 4      : temperature state, 0: normal, 1: abnormal
 *  bit 5...6  : motor voltage state, 00b normal
 *                                    01b over voltage
 *                                    10b under voltage
 *                                    11b reserved
 *  bit 7       : motor blocked, 0: normal, 1: block
 * ---------------------------------------------------------------------
 *  bit 8       : hall sensor state, 0: normal, 1: abnormal
 *  bit 9       : position sensor state, 0: normal, 1: abnormal
 *  bit 10      : current sensor state, 0: normal, 1: abnormal
 *  bit 11      : voltage sensor state, 0: normal, 1: abnormal
 *  bit 12      : temp sensor state, 0: normal, 1: abnormal
 *  bit 13      : commute state, 0: normal
 *                               1: abnormal(60ms no commute received)
 *  bit 14...15 : redundant state, 00b normal
 *                                 01b warning
 *                                 10b fatal
 *                                 11b reserved
 * ---------------------------------------------------------------------
 *
 * @details cpu state
 * ---------------------------------------------------------------------
 *  bit 0 : RVDT state, 0: normal, 1: abnormal
 *  bit 1 : DSP state,  0: normal, 1: abnormal
 *  bit 2 : HAL state,  0: normal, 1: abnormal
 *  bit 3 : under voltage, 0: normal, 1: abnormal
 *  bit 4 : internal power supply, 0: normal, 1: abnormal
 *  bit 5 : adc state, 0: normal, 1: abnormal
 *  bit 6...7 : run state, 00b normal
 *                         01b diagnosis fatal
 *                         10b open loop fatal
 *                         11b motor stop fatal
 * ---------------------------------------------------------------------
 */
struct TxFrameBuilder {
  TxFrameBuilder();

  enum class MotorMode : uint8_t {
    REVERSE_TORQUE = 0,
    POSITION = 1,
    TORQUE = 2,
    VELOCITY = 3,
  };

  enum class MotorVolState : uint8_t {
    NORMAL = 0,
    OVER_VOLTAGE = 1,
    UNDER_VOLTAGE = 2,
    RESERVED = 3,
  };

  enum class RedundantState : uint8_t {
    NORMAL = 0,
    WARNING = 1,
    FATAL = 2,
    RESERVED = 3,
  };

  enum class RunState : uint8_t {
    NORMAL = 0,
    DIAGNOSIS_FATAL = 1,
    OPEN_LOOP_FATAL = 2,
    MOTOR_STOP_FATAL = 3,
  };

  void setCounter(uint8_t counter);
  void setSoftVer(uint8_t version);
  void setStateMotorEnable(bool enabled);
  void setStateMotorMode(MotorMode mode);
  void setStateCurrentState(bool abnormal);
  void setStateTempState(bool abnormal);
  void setStateMotorVoltageState(MotorVolState state);
  void setStateMotorBlocked(bool blocked);
  void setStateMotorHallSensorState(bool abnormal);
  void setStateMotorPositionSensorState(bool abnormal);
  void setStateMotorCurrentSensorState(bool abnormal);
  void setStateMotorVoltageSensorState(bool abnormal);
  void setStateMotorTempSensorState(bool abnormal);
  void setStateMotorCommuteState(bool abnormal);
  void setStateMotorRedundantState(RedundantState state);
  void setStateRvdtState(bool abnormal);
  void setStateDspState(bool abnormal);
  void setStateHallState(bool abnormal);
  void setStateUnderVoltage(bool abnormal);
  void setStateInternalPowerSupply(bool abnormal);
  void setStateAdcState(bool abnormal);
  void setStateRunState(RunState state);
  void setCurrPosition(float pos);
  void setCurrVelocity(float vel);
  void setCurrCurrent(float curr);
  void setCurrMotorTemp(float temp);
  void setCurrMotorVoltage(float vol);
  void setTargetPosition(float pos);
  void setTargetVelocity(float vel);
  void setElectricalAngleOffset(float offset);

  std::span<uint8_t> build();

 private:
  uint8_t mBuf[24];
};

}  // namespace td2kw_detail
}  // namespace proto
}  // namespace coriander
