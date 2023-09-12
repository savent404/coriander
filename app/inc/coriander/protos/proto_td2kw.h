/**
 * @file proto_td2kw.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-19
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once

#include <memory>

#include "boost/circular_buffer.hpp"
#include "coriander/application/diagnosis.h"
#include "coriander/base/ilogger.h"
#include "coriander/base/itty.h"
#include "coriander/iboard_event.h"
#include "coriander/iprotocol_ctl.h"
#include "coriander/os/isystick.h"
#include "coriander/parameters.h"
#include "coriander/protos/proto_td2kw_raw.h"

namespace coriander {
namespace proto {

struct ProtoTd2kw : public IProtocolCtl {
  using ITty = base::ITty;
  using Param = coriander::Parameter;
  using ParamId = coriander::base::ParamId;
  using BoardEvent = coriander::IBoardEvent::Event;
  using Systick = coriander::os::ISystick;
  using Logger = coriander::base::ILogger;
  using Diagnosis = coriander::application::Diagnosis;
  ProtoTd2kw(std::shared_ptr<ITty> tty, std::shared_ptr<IBoardEvent> boardEvent,
             std::shared_ptr<Param> params, std::shared_ptr<Systick> systick,
             std::shared_ptr<Logger> logger,
             std::shared_ptr<Diagnosis> diagnosis);

  void enable() noexcept override;
  void disable() noexcept override;
  void loop() noexcept override;

 private:
  void txRoutine() noexcept;
  void rxRoutine() noexcept;
  void updateReportDeadline() noexcept;
  bool reportDeadlineExpired() const noexcept;
  void updateRecvDeadline() noexcept;
  bool recvDeadlineExpired() const noexcept;

  std::shared_ptr<ITty> mTty;
  std::shared_ptr<IBoardEvent> mBoardEvent;
  std::shared_ptr<Param> mParams;
  std::shared_ptr<Systick> mSystick;
  std::shared_ptr<Logger> mLogger;
  std::shared_ptr<Diagnosis> mDiagnosis;

  td2kw_detail::RxFrameParser mRxFrameParser;
  td2kw_detail::TxFrameBuilder mTxFrameBuilder;
  bool mEnable = false;
  float mCurrTargetPos = -1;
  const uint32_t mReportInterval = 20;
  const uint32_t mRecvTimeout = 60;
  uint32_t mReportDeadline = 0;
  uint32_t mRecvDeadline = 0;
  bool mRecvDeadlineWarned = false;
  int16_t mRxCyclicCounter = -1;
  uint8_t mTxCyclicCounter = 0;
};

}  // namespace proto
}  // namespace coriander
