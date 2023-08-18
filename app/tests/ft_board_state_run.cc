/**
 * @file ft_board_state_run.cc
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-06
 *
 * Copyright 2023 savent_gate
 *
 */
#include <gtest/gtest.h>

#include "boost/di.hpp"
#include "coriander/application/iappstatus.h"
#include "coriander/base/const_hash.h"
#include "coriander/base/ilogger.h"
#include "coriander/base/property.h"
#include "coriander/board_state_run_handler.h"
#include "coriander/coriander.h"
#include "coriander/motorctl/foc_motor_driver.h"
#include "coriander/motorctl/iencoder.h"
#include "coriander/motorctl/imotorctl.h"
#include "coriander/motorctl/motor_ctl_velocity.h"
#include "coriander/motorctl/motor_ctl_wrapper.h"
#include "coriander/os/isystick.h"
#include "coriander/parameters.h"
#include "tests/mocks.h"
#include "posix/posix_mutex.h"
#include "posix/posix_semaphore.h"
#include "posix/posix_thread.h"

namespace {
using coriander::base::operator""_hash;

struct MockIMotorCtl : public coriander::motorctl::IMotorCtl {
  MOCK_METHOD0(start, void());
  MOCK_METHOD0(stop, void());
  MOCK_METHOD0(loop, void());
  MOCK_METHOD0(emergencyStop, void());
  MOCK_METHOD0(fatalError, bool());
};
static auto createInjector() {
  using boost::di::bind;
  using testing::mock::MockAppStatus;
  using testing::mock::MockFocMotorDriver;
  return boost::di::make_injector(
      bind<coriander::application::IAppStatus>.to<MockAppStatus>(),
      bind<coriander::base::ILogger>.to<testing::mock::MockLogger>(),
      bind<coriander::os::ISystick>.to<testing::mock::MockSystick>(),
      bind<coriander::os::IThread>.to<testing::mock::MockThread>(),
      bind<coriander::os::IMutex>.to<coriander::os::posix::Mutex>(),
      bind<coriander::os::ISemaphore>.to<coriander::os::posix::Semaphore>(),
      bind<coriander::motorctl::IEncoder>.to<testing::mock::MockEncoder>(),
      bind<coriander::motorctl::FocMotorDriver>.to<MockFocMotorDriver>());
}
}  // namespace

TEST(BoardStateRun, basic) {
  using ParamId = coriander::base::ParamId;
  using Property = coriander::base::Property;
  using AppStatus = coriander::application::IAppStatus::Status;
  auto c = coriander::coriander_create_injector(createInjector());

  auto param = c.create<std::shared_ptr<coriander::ParameterBase>>();
  param->add(Property{1, ParamId::MotorCtlMode});
  param->add(Property{1.0f, ParamId::VelocityPidP});
  param->add(Property{0.0f, ParamId::VelocityPidI});
  param->add(Property{0.0f, ParamId::VelocityPidD});
  param->add(Property{99999.0f, ParamId::VelocityPidOutputRamp});
  param->add(Property{99999.0f, ParamId::VelocityPidLimit});
  param->add(Property{600.0f, ParamId::TargetVelocity});
  param->add(Property{16.0f, ParamId::MotorSupplyVoltage});
  param->add(Property(16, ParamId::VelocitySampleWindowSize));
  param->add(Property{3500, ParamId::VelocitySampleWindowTime});
  param->add(Property{50, ParamId::VelocitySampleMinimalDuration});
  param->add(Property{4, ParamId::PolePair});
  param->add(Property{
      static_cast<int>(coriander::motorctl::DynamicMotorCtl::Mode::Velocity),
      ParamId::MotorCtlMode});

  auto encoder = c.create<std::shared_ptr<testing::mock::MockEncoder>>();

  auto runHandler =
      c.create<std::shared_ptr<coriander::IBoardStateRunHandler>>();

  EXPECT_CALL(*encoder, sync()).Times(1);
  runHandler->onEnter();
  runHandler->onLoop();
}
