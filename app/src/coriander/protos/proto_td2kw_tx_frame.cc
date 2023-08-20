/**
 * @file proto_td2kw_tx_frame.cc
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-20
 *
 * Copyright 2023 savent_gate
 *
 */
#include "coriander/protos/proto_td2kw.h"

namespace coriander {
namespace proto {
namespace td2kw_detail {

TxFrameBuilder::TxFrameBuilder() {
  for (uint8_t& i : mBuf) {
    i = 0xFF;  // mark as invalid for debug purpose
  }
  mBuf[0] = 0xEB;
  mBuf[1] = 0x90;
}

void TxFrameBuilder::setCounter(uint8_t counter) { mBuf[2] = counter; }
void TxFrameBuilder::setSoftVer(uint8_t version) { mBuf[3] = version; }
void TxFrameBuilder::setStateMotorEnable(bool enabled) {
  uint8_t& p = mBuf[4];
  uint8_t v = enabled ? 1 : 0;
  p &= ~(1 << 0);
  p |= v;
}
void TxFrameBuilder::setStateMotorMode(MotorMode mode) {
  uint8_t& p = mBuf[4];
  uint8_t v = static_cast<uint8_t>(mode);
  p &= ~(0b11 << 1);
  p |= v << 1;
}
void TxFrameBuilder::setStateCurrentState(bool abnormal) {
  uint8_t& p = mBuf[4];
  uint8_t v = abnormal ? 1 : 0;
  p &= ~(1 << 3);
  p |= v << 3;
}
void TxFrameBuilder::setStateTempState(bool abnormal) {
  uint8_t& p = mBuf[4];
  uint8_t v = abnormal ? 1 : 0;
  p &= ~(1 << 4);
  p |= v << 4;
}
void TxFrameBuilder::setStateMotorVoltageState(MotorVolState state) {
  uint8_t& p = mBuf[4];
  uint8_t v = static_cast<uint8_t>(state);
  p &= ~(0b11 << 5);
  p |= v << 5;
}
void TxFrameBuilder::setStateMotorBlocked(bool blocked) {
  uint8_t& p = mBuf[4];
  uint8_t v = blocked ? 1 : 0;
  p &= ~(1 << 0);
  p |= v << 7;
}
void TxFrameBuilder::setStateMotorHallSensorState(bool abnormal) {
  uint8_t& p = mBuf[5];
  uint8_t v = abnormal ? 1 : 0;
  p &= ~(1 << 0);
  p |= v;
}
void TxFrameBuilder::setStateMotorPositionSensorState(bool abnormal) {
  uint8_t& p = mBuf[5];
  uint8_t v = abnormal ? 1 : 0;
  p &= ~(1 << 1);
  p |= v << 1;
}
void TxFrameBuilder::setStateMotorCurrentSensorState(bool abnormal) {
  uint8_t& p = mBuf[5];
  uint8_t v = abnormal ? 1 : 0;
  p &= ~(1 << 2);
  p |= v << 2;
}
void TxFrameBuilder::setStateMotorVoltageSensorState(bool abnormal) {
  uint8_t& p = mBuf[5];
  uint8_t v = abnormal ? 1 : 0;
  p &= ~(1 << 3);
  p |= v << 3;
}
void TxFrameBuilder::setStateMotorTempSensorState(bool abnormal) {
  uint8_t& p = mBuf[5];
  uint8_t v = abnormal ? 1 : 0;
  p &= ~(1 << 4);
  p |= v << 4;
}
void TxFrameBuilder::setStateMotorCommuteState(bool abnormal) {
  uint8_t& p = mBuf[5];
  uint8_t v = abnormal ? 1 : 0;
  p &= ~(1 << 5);
  p |= v << 5;
}
void TxFrameBuilder::setStateMotorRedundantState(RedundantState state) {
  uint8_t& p = mBuf[5];
  uint8_t v = static_cast<uint8_t>(state);
  p &= ~(0b11 << 6);
  p |= v << 6;
}
void TxFrameBuilder::setStateRvdtState(bool abnormal) {
  uint8_t& p = mBuf[6];
  uint8_t v = abnormal ? 1 : 0;
  p &= ~(1 << 0);
  p |= v;
}
void TxFrameBuilder::setStateDspState(bool abnormal) {
  uint8_t& p = mBuf[6];
  uint8_t v = abnormal ? 1 : 0;
  p &= ~(1 << 1);
  p |= v << 1;
}
void TxFrameBuilder::setStateHallState(bool abnormal) {
  uint8_t& p = mBuf[6];
  uint8_t v = abnormal ? 1 : 0;
  p &= ~(1 << 2);
  p |= v << 2;
}
void TxFrameBuilder::setStateUnderVoltage(bool abnormal) {
  uint8_t& p = mBuf[6];
  uint8_t v = abnormal ? 1 : 0;
  p &= ~(1 << 3);
  p |= v << 3;
}
void TxFrameBuilder::setStateInternalPowerSupply(bool abnormal) {
  uint8_t& p = mBuf[6];
  uint8_t v = abnormal ? 1 : 0;
  p &= ~(1 << 4);
  p |= v << 4;
}
void TxFrameBuilder::setStateAdcState(bool abnormal) {
  uint8_t& p = mBuf[6];
  uint8_t v = abnormal ? 1 : 0;
  p &= ~(1 << 5);
  p |= v << 5;
}
void TxFrameBuilder::setStateRunState(RunState state) {
  uint8_t& p = mBuf[6];
  uint8_t v = static_cast<uint8_t>(state);
  p &= ~(0b11 << 6);
  p |= v << 6;
}
void TxFrameBuilder::setCurrPosition(float pos) {
  uint8_t& pl = mBuf[7];
  uint8_t& ph = mBuf[8];
  uint16_t v = static_cast<uint16_t>(pos * 10);

  // range 0 ~ 360 degree
  if (v >= 3600) {
    v = 3600;
  }
  pl = v & 0xFF;
  ph = (v >> 8) & 0xFF;
}
void TxFrameBuilder::setCurrVelocity(float vel) {
  uint8_t& pl = mBuf[9];
  uint8_t& ph = mBuf[10];
  int16_t v = static_cast<uint16_t>(vel * 10);
  pl = v & 0xFF;
  ph = (v >> 8) & 0xFF;
}
void TxFrameBuilder::setCurrCurrent(float curr) {
  uint8_t& pl = mBuf[11];
  uint8_t& ph = mBuf[12];
  int16_t v = static_cast<uint16_t>(curr * 100);

  // range: -50A ~ 50A
  if (v >= 5000) {
    v = 5000;
  } else if (v <= -5000) {
    v = -5000;
  }
  pl = v & 0xFF;
  ph = (v >> 8) & 0xFF;
}
void TxFrameBuilder::setCurrMotorTemp(float temp) {
  uint8_t& pl = mBuf[13];
  uint8_t& ph = mBuf[14];
  int16_t v = static_cast<uint16_t>(temp * 10);

  // range: -50 ~ 300 degree
  if (v >= 3000) {
    v = 3000;
  } else if (v <= -500) {
    v = -500;
  }
  pl = v & 0xFF;
  ph = (v >> 8) & 0xFF;
}
void TxFrameBuilder::setCurrMotorVoltage(float vol) {
  uint8_t& pl = mBuf[15];
  uint8_t& ph = mBuf[16];
  uint16_t v = static_cast<uint16_t>(vol * 10);

  // range: 0 ~ 300V
  if (v >= 3000) {
    v = 3000;
  }
  pl = v & 0xFF;
  ph = (v >> 8) & 0xFF;
}
void TxFrameBuilder::setTargetPosition(float pos) {
  uint8_t& pl = mBuf[17];
  uint8_t& ph = mBuf[18];
  uint16_t v = static_cast<uint16_t>(pos * 10);

  // range: 0 ~ 360 degree
  if (v >= 3600) {
    v = 3600;
  }
  pl = v & 0xFF;
  ph = (v >> 8) & 0xFF;
}
void TxFrameBuilder::setTargetVelocity(float vel) {
  uint8_t& pl = mBuf[19];
  uint8_t& ph = mBuf[20];
  int16_t v = static_cast<uint16_t>(vel * 10);
  pl = v & 0xFF;
  ph = (v >> 8) & 0xFF;
}
void TxFrameBuilder::setElectricalAngleOffset(float offset) {
  uint8_t& pl = mBuf[21];
  uint8_t& ph = mBuf[22];
  int16_t v = static_cast<uint16_t>(offset * 10);
  pl = v & 0xFF;
  ph = (v >> 8) & 0xFF;
}

std::span<uint8_t> TxFrameBuilder::build() {
  // cal checksum
  uint8_t checksum = 0;
  for (size_t i = 2; i < sizeof(mBuf) - 1; i++) {
    checksum += mBuf[i];
  }
  mBuf[sizeof(mBuf) - 1] = checksum;
  return std::span<uint8_t>(mBuf, sizeof(mBuf));
}

}  // namespace td2kw_detail
}  // namespace proto
}  // namespace coriander
