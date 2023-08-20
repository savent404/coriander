/**
 * @file proto_td2kw.cc
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

RxFrameParser::RxFrameParser() : mBuf(16) {}

bool RxFrameParser::parse(std::span<uint8_t> buf) {
  for (auto byte : buf) {
    if (parseOneByte(byte)) {
      return true;
    }
  }
  return false;
}

bool RxFrameParser::parseOneByte(uint8_t byte) {
  ssize_t cur_idx = mBuf.size();
  bool accept = false;

  switch (cur_idx) {
    case 0:
      if (byte == 0xEB) {
        accept = true;
      }
      break;
    case 1:
      if (byte == 0x90) {
        accept = true;
      }
      break;
    case 2 ... 11:
      accept = true;
      break;
    case 12:
      if (byte == getChecksum()) {
        accept = true;
      }
      break;
    default:
      break;
  }

  if (!accept) {
    /** discard all bytes until header1(0xEB)
     * NOTE(savent): there is possible that the header2 is invalid
     *               and it just waste time to discard again, but
     *               it's not a big deal cause no data loss.
     */
    while (mBuf.size() && mBuf[0] != 0xEB) {
      mBuf.pop_front();
    }
  } else {
    mBuf.push_back(byte);
  }

  // byte accepted and frame is complete
  return accept && mBuf.size() == 13;
}

uint8_t RxFrameParser::getChecksum() const {
  uint8_t checksum = 0;
  if (mBuf.size() != 12) {
    return checksum;
  }
  // checksum = sum of [2...11](exclude checksum and sync bytes)
  for (int idx = 2; idx < 11; ++idx) {
    checksum += mBuf.at(idx);
  }
  return checksum;
}

uint8_t RxFrameParser::getCounter() const { return mBuf.at(2); }

bool RxFrameParser::getEnable() const {
  uint8_t b = mBuf.at(3);
  bool res;
  switch (b) {
    case 0xF0:
      res = true;
      break;
    case 0x0F:
    default:
      res = false;
      break;
  }
  return res;
}

float RxFrameParser::getTargetPosition() const {
  uint8_t bl = mBuf.at(4);
  uint8_t bh = mBuf.at(5);
  uint16_t b = (bh << 8) | bl;
  auto f = static_cast<float>(b) / 10.0f;  // lsb 0.1 degree
  return f;
}

}  // namespace td2kw_detail

ProtoTd2kw::ProtoTd2kw(std::shared_ptr<ITty> tty,
                       std::shared_ptr<IBoardEvent> boardEvent,
                       std::shared_ptr<Param> param,
                       std::shared_ptr<Systick> systick,
                       std::shared_ptr<Logger> logger)
    : mTty(tty),
      mBoardEvent(boardEvent),
      mParams(param),
      mSystick(systick),
      mLogger(logger),
      mLoggerStream(logger) {}

void ProtoTd2kw::enable() noexcept { updateReportDeadline(); }
void ProtoTd2kw::disable() noexcept {}

void ProtoTd2kw::loop() noexcept {
  rxRoutine();
  if (reportDeadlineExpired()) {
    txRoutine();
    updateReportDeadline();
  }
}

void ProtoTd2kw::txRoutine() noexcept {}

void ProtoTd2kw::rxRoutine() noexcept {
  uint8_t buf[16];
  ssize_t n;
  float newTargetPos;
  bool parseRes, targetPosChanged, enableTriggered, disableTriggered;

  n = mTty->read(buf, sizeof(buf));
  if (n < 0) {
    return;
  }

  parseRes = mRxFrameParser.parse(std::span<uint8_t>(buf, n));
  if (!parseRes) {
    return;
  }

  if (mRxFrameParser.getCounter() != mRxCyclicCounter &&
      mRxCyclicCounter != -1) {
    mLoggerStream << "proto_td2kw: rx frame lost, wanted counter: "
                  << mRxCyclicCounter
                  << ", got counter: " << mRxFrameParser.getCounter()
                  << std::endl;
  }

  newTargetPos = mRxFrameParser.getTargetPosition();
  targetPosChanged = newTargetPos != mCurrTargetPos;
  enableTriggered = mRxFrameParser.getEnable() && !mEnable;
  disableTriggered = !mRxFrameParser.getEnable() && mEnable;
  mRxCyclicCounter = mRxFrameParser.getCounter();

  // internal state update
  mEnable = mRxFrameParser.getEnable();
  mCurrTargetPos = newTargetPos;

  // update target position
  if (targetPosChanged && mEnable) {
    mParams->setValue(ParamId::MotorCtl_General_TargetPosition_RT,
                      mCurrTargetPos);
    if (!enableTriggered && !disableTriggered) {
      mBoardEvent->raiseEvent(BoardEvent::ParameterUpdate);
    }
  }

  // update enable state
  if (enableTriggered) {
    // enforce update target position
    mParams->setValue(ParamId::MotorCtl_General_TargetPosition_RT,
                      newTargetPos);
    mBoardEvent->raiseEvent(BoardEvent::MotorStart);
  } else if (disableTriggered) {
    mBoardEvent->raiseEvent(BoardEvent::MotorStop);
  }
}

void ProtoTd2kw::updateReportDeadline() noexcept {
  mReportDeadline = mSystick->systick_ms() + mReportInterval;
}

bool ProtoTd2kw::reportDeadlineExpired() const noexcept {
  return mSystick->systick_ms() >= mReportDeadline;
}

}  // namespace proto
}  // namespace coriander
