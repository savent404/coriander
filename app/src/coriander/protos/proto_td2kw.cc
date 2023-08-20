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
