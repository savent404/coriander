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

namespace {
using namespace coriander;
using namespace coriander::motorctl;
using namespace coriander::base;
using namespace coriander::os;

struct MockBldcDriver : public IBldcDriver {
  MOCK_METHOD0(enable, void());
  MOCK_METHOD0(disable, void());
  MOCK_METHOD3(setVoltage, void(uint16_t, uint16_t, uint16_t));
  MOCK_METHOD0(emergencyBreak, void());
};

struct MockElecAngleEstimator : public IElecAngleEstimator {
  MOCK_METHOD(float, getElectricalAngle, (), (noexcept));
  MOCK_METHOD0(enable, void());
  MOCK_METHOD0(disable, void());
  MOCK_METHOD0(enabled, bool());
  MOCK_METHOD0(sync, void());
  MOCK_METHOD0(calibrate, void());
  MOCK_METHOD0(needCalibrate, bool());
};

struct MockSystick : public ISystick {
  MOCK_METHOD0(systick_ms, uint32_t());
  MOCK_METHOD0(systick_us, uint32_t());
};
struct MockBoardEvent : public IBoardEvent {
  MOCK_METHOD(void, raiseEvent, (Event), (noexcept));
  MOCK_METHOD(void, registerEventCallback, (Event, EventCallback), (noexcept));
};

struct MockLogger : public base::ILogger {
  MOCK_METHOD(void, log, (const char* msg), (noexcept));
};

}  // namespace

using namespace boost::di;

static auto createInjector() {
  return make_injector(bind<IBldcDriver>().to<MockBldcDriver>(),
                       bind<IElecAngleEstimator>().to<MockElecAngleEstimator>(),
                       bind<ISystick>().to<MockSystick>(),
                       bind<ParameterBase>().to<ParameterBase>(),
                       bind<ILogger>().to<MockLogger>(),
                       bind<IBoardEvent>().to<MockBoardEvent>(),
                       bind<IMotorCtl>().to<MotorCtlCalibrate>());
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

  param->add(Property{5.0f, "calibrate_voltage"});
  param->add(Property{3000, "calibrate_duration"});
  param->add(Property{16.0f, "motor_supply_voltage"});

  auto mc = c.create<std::shared_ptr<IMotorCtl>>();

  EXPECT_CALL(*motor, enable()).Times(1);
  EXPECT_CALL(*elecAngle, enable()).Times(1);
  EXPECT_CALL(*elecAngle, enabled()).Times(1).WillOnce(Return(false));
  EXPECT_CALL(*systick, systick_ms()).Times(1).WillOnce(Return(0));
  EXPECT_CALL(*motor, setVoltage(uint16_t(5 / 16.0f * UINT16_MAX), 0, 0))
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
