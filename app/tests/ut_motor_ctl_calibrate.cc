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
#include "coriander/motorctl/iphase_current_estimator.h"
#include "coriander/motorctl/motor_ctl_calibrate.h"
#include "coriander/os/isystick.h"
#include "coriander/parameters.h"
#include "posix/posix_logger.h"
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
using coriander::Parameter;
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
      bind<Parameter>().to<testing::mock::MockPersistentParameter>(),
      bind<ILogger>().to<coriander::base::posix::Logger>(),
      bind<IBoardEvent>().to<MockBoardEvent>(),
      bind<IMotorCtl>().to<MotorCtlCalibrate>(),
      bind<coriander::motorctl::IPhaseCurrentEstimator>()
          .to<testing::mock::MockPhaseCurrentEstimator>(),
      bind<IParamReqValidator>().to<testing::mock::MockParamReqValidator>());
}

using testing::Return;
TEST(MotorCtl, calibrate) {
  auto c = createInjector();

  // prepare parameters
  auto param = c.create<std::shared_ptr<Parameter>>();
  auto motor = c.create<std::shared_ptr<MockBldcDriver>>();
  auto elecAngle = c.create<std::shared_ptr<MockElecAngleEstimator>>();
  auto systick = c.create<std::shared_ptr<MockSystick>>();
  auto boardEvent = c.create<std::shared_ptr<MockBoardEvent>>();

  param->add(Property{5.0f, ParamId::MotorCtl_Calibrate_CaliVoltage});
  param->add(Property{3000, ParamId::MotorCtl_Calibrate_CaliDuration});
  param->add(Property{16.0f, ParamId::MotorCtl_MotorDriver_SupplyVoltage});

  auto mc = c.create<std::shared_ptr<IMotorCtl>>();

  EXPECT_CALL(*motor, enable()).Times(1);
  EXPECT_CALL(*elecAngle, enable()).Times(1);
  EXPECT_CALL(*elecAngle, enabled()).Times(1).WillOnce(Return(false));
  EXPECT_CALL(
      *motor,
      setPhaseDutyCycle(static_cast<uint16_t>(UINT16_MAX * 5.0f / 16.0f), 0, 0))
      .Times(1);
  mc->start();

  EXPECT_CALL(*elecAngle, needCalibrate())
      .Times(2)
      .WillOnce(Return(true))
      .WillOnce(Return(false));

  EXPECT_CALL(*elecAngle, sync()).Times(4);
  EXPECT_CALL(*elecAngle, calibrate()).Times(4);  // 3s
  EXPECT_CALL(*systick, systick_ms())
      .Times(6)
      .WillOnce(Return(0))
      .WillOnce(Return(1000))
      .WillOnce(Return(2000))
      .WillOnce(Return(3000))
      .WillOnce(Return(4000))
      .WillOnce(Return(5000));
  // trigger CalibrateDone event
  EXPECT_CALL(*boardEvent, raiseEvent(IBoardEvent::Event::CalibrateDone))
      .Times(1);

  for (int i = 0; i < 9; i++) {
    mc->loop();
  }

  // mc disable
  EXPECT_CALL(*elecAngle, enabled()).Times(1).WillOnce(Return(true));
  EXPECT_CALL(*elecAngle, disable()).Times(1);
  mc->stop();
}
