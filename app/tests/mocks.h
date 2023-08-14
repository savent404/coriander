/**
 * @file mocks.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief General mocks for unit tests
 * @date 2023-08-05
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once

#include <gmock/gmock.h>

#include <memory>

#include "coriander/application/iappstatus.h"
#include "coriander/base/ilogger.h"
#include "coriander/iboard_event.h"
#include "coriander/iboard_state.h"
#include "coriander/iboard_state_calibrate_handler.h"
#include "coriander/iboard_state_error_handler.h"
#include "coriander/iboard_state_firmware_update_handler.h"
#include "coriander/iboard_state_init_handler.h"
#include "coriander/iboard_state_reboot_handler.h"
#include "coriander/iboard_state_run_handler.h"
#include "coriander/iboard_state_standby_handler.h"
#include "coriander/iprotocol_ctl.h"
#include "coriander/istate_handler.h"
#include "coriander/motorctl/foc_motor_driver.h"
#include "coriander/motorctl/ibldc_driver.h"
#include "coriander/motorctl/ielec_angle_estimator.h"
#include "coriander/motorctl/iencoder.h"
#include "coriander/motorctl/imech_angle_estimator.h"
#include "coriander/os/isemaphore.h"
#include "coriander/os/isystick.h"
#include "coriander/os/ithread.h"
#include "coriander/parameter_requirements.h"
#include "coriander/parameter_requirements_validator.h"

namespace testing {
namespace mock {

using namespace coriander;

struct MockBoardEvent : public IBoardEvent {
  MOCK_METHOD(void, raiseEvent, (Event), (noexcept));
  MOCK_METHOD(void, registerEventCallback, (Event, EventCallback), (noexcept));
  MOCK_METHOD0(eventLock, void());
  MOCK_METHOD0(eventUnlock, void());
};

struct MockLogger : public base::ILogger {
  MOCK_METHOD(void, log, (const char* msg), (noexcept));
};

struct MockSystick : public os::ISystick {
  MOCK_METHOD(uint32_t, systick_ms, (), (noexcept));
  MOCK_METHOD(uint32_t, systick_us, (), (noexcept));
};

struct MockBldcDriver : public motorctl::IBldcDriver {
  MOCK_METHOD0(enable, void());
  MOCK_METHOD0(disable, void());
  MOCK_METHOD3(setPhaseDutyCycle, void(uint16_t, uint16_t, uint16_t));
  MOCK_METHOD0(emergencyBreak, void());
};

struct MockElecAngleEstimator : public motorctl::IElecAngleEstimator {
  MOCK_METHOD(float, getElectricalAngle, (), (noexcept));
  MOCK_METHOD0(enable, void());
  MOCK_METHOD0(disable, void());
  MOCK_METHOD0(enabled, bool());
  MOCK_METHOD0(sync, void());
  MOCK_METHOD0(calibrate, void());
  MOCK_METHOD0(needCalibrate, bool());
};

struct MockMechAngleEstimator : public motorctl::IMechAngleEstimator {
  MOCK_METHOD(float, getMechanicalAngle, (), (noexcept));
  MOCK_METHOD0(enable, void());
  MOCK_METHOD0(disable, void());
  MOCK_METHOD0(enabled, bool());
  MOCK_METHOD0(sync, void());
  MOCK_METHOD1(needSync, bool(uint32_t));
  MOCK_METHOD0(needCalibrate, bool());
  MOCK_METHOD0(calibrate, void());
};

struct MockEncoder : public motorctl::IEncoder {
  MOCK_METHOD0(getEncoderCount, unsigned());
  MOCK_METHOD0(getEncoderCountPerRound, unsigned());
  MOCK_METHOD0(getOverflowCount, int());
  MOCK_METHOD0(enable, void());
  MOCK_METHOD0(disable, void());
  MOCK_METHOD0(enabled, bool());
  MOCK_METHOD0(sync, void());
  MOCK_METHOD0(needCalibrate, bool());
  MOCK_METHOD0(calibrate, void());
};

struct MockAppStatus : public application::IAppStatus {
  MOCK_METHOD(void, setStatus, (Status), (noexcept));
  MOCK_METHOD(Status, getStatus, (), (const, noexcept));
};

struct MockStateHandler : public IStateHandler {
  MOCK_METHOD(void, onEnter, (), (noexcept));
  MOCK_METHOD(void, onExit, (), (noexcept));
  MOCK_METHOD(void, onLoop, (), (noexcept));
};

struct MockBoardStateInitHandler : public IBoardStateInitHandler {
  MOCK_METHOD(void, onEnter, (), (noexcept));
  MOCK_METHOD(void, onExit, (), (noexcept));
  MOCK_METHOD(void, onLoop, (), (noexcept));
};

struct MockBoardStateStandbyHandler : public IBoardStateStandbyHandler {
  MOCK_METHOD(void, onEnter, (), (noexcept));
  MOCK_METHOD(void, onExit, (), (noexcept));
  MOCK_METHOD(void, onLoop, (), (noexcept));
};

struct MockBoardStateRunHandler : public IBoardStateRunHandler {
  MOCK_METHOD(void, onEnter, (), (noexcept));
  MOCK_METHOD(void, onExit, (), (noexcept));
  MOCK_METHOD(void, onLoop, (), (noexcept));
};

struct MockBoardStateErrorHandler : public IBoardStateErrorHandler {
  MOCK_METHOD(void, onEnter, (), (noexcept));
  MOCK_METHOD(void, onExit, (), (noexcept));
  MOCK_METHOD(void, onLoop, (), (noexcept));
};

struct MockBoardStateCalibrateHandler : public IBoardStateCalibrateHandler {
  MOCK_METHOD(void, onEnter, (), (noexcept));
  MOCK_METHOD(void, onExit, (), (noexcept));
  MOCK_METHOD(void, onLoop, (), (noexcept));
};

struct MockBoardStateFirmwareUpdateHandler
    : public IBoardStateFirmwareUpdateHandler {
  MOCK_METHOD(void, onEnter, (), (noexcept));
  MOCK_METHOD(void, onExit, (), (noexcept));
  MOCK_METHOD(void, onLoop, (), (noexcept));
};

struct MockBoardStateRebootHandler : public IBoardStateRebootHandler {
  MOCK_METHOD(void, onEnter, (), (noexcept));
  MOCK_METHOD(void, onExit, (), (noexcept));
  MOCK_METHOD(void, onLoop, (), (noexcept));
};

struct MockFocMotorDriver : public coriander::motorctl::FocMotorDriver {
  MockFocMotorDriver(std::shared_ptr<coriander::motorctl::IElecAngleEstimator>
                         elecAngleEstimator)
      : coriander::motorctl::FocMotorDriver(elecAngleEstimator) {}
  MOCK_METHOD0(enable, void());
  MOCK_METHOD0(disable, void());
  MOCK_METHOD0(emergencyBreak, void());
  MOCK_METHOD0(fatalError, bool());
  MOCK_METHOD(void, setPhaseDutyCycle, (uint16_t, uint16_t, uint16_t),
              (override));
  // void setPhaseDutyCycle(uint16_t u, uint16_t v, uint16_t w) override {}
};

struct MockSemaphore : public os::ISemaphore {
  MOCK_METHOD(void, wait, (), (noexcept));
  MOCK_METHOD(bool, tryWait, (), (noexcept));
  MOCK_METHOD(void, post, (), (noexcept));
};

struct MockThread : public os::IThread {
  MOCK_METHOD(size_t, currentThreadId, (), (noexcept));
};

struct MockProtoCtl : public IProtocolCtl {
  MOCK_METHOD(void, enable, (), (noexcept));
  MOCK_METHOD(void, disable, (), (noexcept));
  MOCK_METHOD(void, loop, (), (noexcept));
};

struct MockParamReqValidator : public IParamReqValidator {
  MOCK_METHOD(void, addParamReq, (IParamReq*));
  MOCK_METHOD(bool, validate, ());
};

}  // namespace mock
}  // namespace testing