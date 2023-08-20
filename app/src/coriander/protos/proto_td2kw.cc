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
                       std::shared_ptr<Logger> logger,
                       std::shared_ptr<Diagnosis> diagnosis)
    : mTty(tty),
      mBoardEvent(boardEvent),
      mParams(param),
      mSystick(systick),
      mLogger(logger),
      mLoggerStream(logger),
      mDiagnosis(diagnosis) {
  using DiagDevId = Diagnosis::DiagDevId;
  using DeviceStatus = Diagnosis::DeviceStatus;
  using RunState = td2kw_detail::TxFrameBuilder::RunState;
  /**
   * @todo add diagnosis state
   *
   * setStateRvdtState(bool abnormal);
   * setStateDspState(bool abnormal);
   * setStateAdcState(bool abnormal);
   * setStateRunState(RunState state);
   */
  mTxFrameBuilder.setStateRvdtState(false);
  mTxFrameBuilder.setStateDspState(false);
  mTxFrameBuilder.setStateAdcState(false);
  mTxFrameBuilder.setStateRunState(RunState::NORMAL);

  mDiagnosis->addDiagInspector([this](DiagDevId dev, DeviceStatus status) {
    bool isAbnormal = status != DeviceStatus::Normal;
    switch (dev) {
      case DiagDevId::DrvBoardTemp:
        mTxFrameBuilder.setStateTempState(isAbnormal);
        break;
      case DiagDevId::DrvBoardPsVoltage:
      case DiagDevId::DrvBoardPsVoltage_UnderCheck:
        mTxFrameBuilder.setStateUnderVoltage(isAbnormal);
        // NOTE(savent): fall-though
      case DiagDevId::DrvBoardPsVoltage_OverCheck: {
        using MotorVolState = td2kw_detail::TxFrameBuilder::MotorVolState;
        MotorVolState volState = MotorVolState::RESERVED;
        if (!isAbnormal) {
          volState = MotorVolState::NORMAL;
        } else if (dev == DiagDevId::DrvBoardPsVoltage_UnderCheck) {
          volState = MotorVolState::UNDER_VOLTAGE;
        } else if (dev == DiagDevId::DrvBoardPsVoltage_OverCheck) {
          volState = MotorVolState::OVER_VOLTAGE;
        } else {
          volState = MotorVolState::NORMAL;
        }
        mTxFrameBuilder.setStateMotorVoltageState(volState);
        break;
      }
      case DiagDevId::DrvBoardPsCurrent:
        mTxFrameBuilder.setStateCurrentState(isAbnormal);
        break;
      case DiagDevId::CtlrBoardPsVoltage:
        mTxFrameBuilder.setStateInternalPowerSupply(isAbnormal);
        break;
      case DiagDevId::MotorBlocked:
        mTxFrameBuilder.setStateMotorBlocked(isAbnormal);
        break;
      case DiagDevId::MotorInnerPosSensor:
        mTxFrameBuilder.setStateHallState(isAbnormal);
        break;
      default:
        break;
    }
  });
}

void ProtoTd2kw::enable() noexcept {
  updateReportDeadline();
  updateRecvDeadline();
}
void ProtoTd2kw::disable() noexcept {}

void ProtoTd2kw::loop() noexcept {
  rxRoutine();

  if (reportDeadlineExpired()) {
    txRoutine();
    updateReportDeadline();
  }
}

void ProtoTd2kw::txRoutine() noexcept {
  using MotorMode = td2kw_detail::TxFrameBuilder::MotorMode;
  auto& b = mTxFrameBuilder;
  float v;

  // NOTE(savent): all state reported by diagnosis inspector which registered
  // in ProtoTd2kw's constructure
  b.setCounter(mTxCyclicCounter++);
  b.setSoftVer(0);
  b.setStateMotorEnable(mEnable);
  b.setStateMotorMode(MotorMode::POSITION);
  v = mParams->getValue<float>(ParamId::Sensor_Mech_Position_RT);
  b.setCurrPosition(v);
  v = mParams->getValue<float>(ParamId::Sensor_Mech_Velocity_RT);
  b.setCurrVelocity(v);
  v = mParams->getValue<float>(ParamId::Sensor_Motor_Current_RT);
  b.setCurrCurrent(v);
  v = mParams->getValue<float>(ParamId::Sensor_Motor_Temp_RT);
  b.setCurrMotorTemp(v);
  v = mParams->getValue<float>(ParamId::Sensor_Motor_Voltage_RT);
  b.setCurrMotorVoltage(v);
  v = mParams->getValue<float>(ParamId::MotorCtl_General_TargetPosition_RT);
  b.setTargetPosition(v);
  v = mParams->getValue<float>(ParamId::MotorCtl_General_TargetVelocity_RT);
  b.setTargetVelocity(v);
  v = mParams->getValue<float>(ParamId::MotorCtl_Calibrate_CaliElecAngleOffset);
  b.setElectricalAngleOffset(v);

  auto msg = b.build();

  auto n = mTty->write(msg.data(), msg.size());
  if (static_cast<size_t>(n) != msg.size()) {
    mLoggerStream << "td2kw: send message failed" << std::endl;
  }
}

void ProtoTd2kw::rxRoutine() noexcept {
  uint8_t buf[16];
  size_t readSize = sizeof(buf);
  ssize_t n;
  float newTargetPos;
  bool parseRes, targetPosChanged, enableTriggered, disableTriggered;

  // limit read size
  if (readSize > mRxFrameParser.requiredBytesSize()) {
    readSize = mRxFrameParser.requiredBytesSize();
  }

  n = mTty->read(buf, readSize);
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

  if (recvDeadlineExpired() && !mRecvDeadlineWarned) {
    mLoggerStream << "proto_td2kw: recv timeout" << std::endl;
    mTxFrameBuilder.setStateMotorCommuteState(true);
    mRecvDeadlineWarned = true;
  } else {
    mTxFrameBuilder.setStateMotorCommuteState(false);
    mRecvDeadlineWarned = false;
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

void ProtoTd2kw::updateRecvDeadline() noexcept {
  mRecvDeadline = mSystick->systick_ms() + mRecvTimeout;
}
bool ProtoTd2kw::recvDeadlineExpired() const noexcept {
  return mSystick->systick_ms() >= mRecvDeadline;
}

}  // namespace proto
}  // namespace coriander
