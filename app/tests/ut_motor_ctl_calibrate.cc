/**
 * @file ut_motor_ctl_calibrate.cc
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-04
 *
 * Copyright 2023 savent_gate
 *
 */
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "boost/di.hpp"
#include "coriander/base/ilogger.h"
#include "coriander/board_event.h"
#include "coriander/iboard_event.h"
#include "coriander/motorctl/ibldc_driver.h"
#include "coriander/motorctl/ielec_angle_estimator.h"
#include "coriander/motorctl/imotorctl.h"
#include "coriander/motorctl/motor_ctl_calibrate.h"
#include "coriander/os/isystick.h"
#include "coriander/parameters.h"
#include "tests/mocks.h"

namespace {
using MockBldcDriver = testing::mock::MockBldcDriver;
using MockElecAngleEstimator = testing::mock::MockElecAngleEstimator;
using MockSystick = testing::mock::MockSystick;
using MockBoardEvent = testing::mock::MockBoardEvent;
using MockLogger = testing::mock::MockLogger;
using MotorCtlCalibrate = coriander::motorctl::MotorCtlCalibrate;
using IMotorCtl = coriander::motorctl::IMotorCtl;
using IParamReqValidator = coriander::IParamReqValidator;
using ParameterBase = coriander::ParameterBase;
using Property = coriander::base::Property;
using ParamId = coriander::base::ParamId;
using IBoardEvent = coriander::IBoardEvent;
using ILogger = coriander::base::ILogger;
using coriander::motorctl::IBldcDriver;
using coriander::motorctl::IElecAngleEstimator;
using coriander::os::ISystick;

}  // namespace

using boost::di::bind;
using ParamId = coriander::base::ParamId;

static auto createInjector() {
  return make_injector(
      bind<IBldcDriver>().to<MockBldcDriver>(),
      bind<IElecAngleEstimator>().to<MockElecAngleEstimator>(),
      bind<ISystick>().to<MockSystick>(),
      bind<ParameterBase>().to<ParameterBase>(),
      bind<ILogger>().to<MockLogger>(),
      bind<IBoardEvent>().to<MockBoardEvent>(),
      bind<IMotorCtl>().to<MotorCtlCalibrate>(),
      bind<IParamReqValidator>().to<testing::mock::MockParamReqValidator>());
}

using testing::Return;
TEST(MotorCtl, calibrate) {
  auto c = createInjector();

  // prepare parameters
  auto param = c.create<std::shared_ptr<ParameterBase>>();
  auto motor = c.create<std::shared_ptr<MockBldcDriver>>();
  auto elecAngle = c.create<std::shared_ptr<MockElecAngleEstimator>>();
  auto systick = c.create<std::shared_ptr<MockSystick>>();
  auto boardEvent = c.create<std::shared_ptr<MockBoardEvent>>();

  param->add(Property{5.0f, ParamId::CalibrateVoltage});
  param->add(Property{3000, ParamId::CalibrateDuration});
  param->add(Property{16.0f, ParamId::MotorSupplyVoltage});

  auto mc = c.create<std::shared_ptr<IMotorCtl>>();

  EXPECT_CALL(*motor, enable()).Times(1);
  EXPECT_CALL(*elecAngle, enable()).Times(1);
  EXPECT_CALL(*elecAngle, enabled()).Times(1).WillOnce(Return(false));
  EXPECT_CALL(*systick, systick_ms()).Times(1).WillOnce(Return(0));
  EXPECT_CALL(
      *motor,
      setPhaseDutyCycle(static_cast<uint16_t>(UINT16_MAX * 5.0f / 16.0f), 0, 0))
      .Times(1);
  mc->start();

  EXPECT_CALL(*elecAngle, sync()).Times(5);
  EXPECT_CALL(*elecAngle, calibrate()).Times(5);
  EXPECT_CALL(*systick, systick_ms())
      .Times(5)
      .WillOnce(Return(600))
      .WillOnce(Return(1200))
      .WillOnce(Return(1800))
      .WillOnce(Return(2400))
      .WillOnce(Return(3000));
  for (int i = 0; i < 5; i++) {
    mc->loop();
  }

  // trigger CalibrateDone event
  EXPECT_CALL(*systick, systick_ms()).Times(1).WillOnce(Return(3001));
  EXPECT_CALL(*boardEvent, raiseEvent(IBoardEvent::Event::CalibrateDone))
      .Times(1);
  EXPECT_CALL(*elecAngle, sync()).Times(1);
  EXPECT_CALL(*elecAngle, calibrate()).Times(1);
  mc->loop();

  // mc disable
  EXPECT_CALL(*elecAngle, enabled()).Times(1).WillOnce(Return(true));
  EXPECT_CALL(*elecAngle, disable()).Times(1);
  EXPECT_CALL(*motor, disable()).Times(1);
  mc->stop();
}
