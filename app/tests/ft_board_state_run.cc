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
#include "posix/posix_mutex.h"
#include "posix/posix_semaphore.h"
#include "posix/posix_thread.h"
#include "tests/mocks.h"

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
  using testing::mock::MockPhaseCurrentEstimator;
  using Mpce = testing::mock::MockPhaseCurrentEstimator;
  return boost::di::make_injector(
      bind<coriander::application::IAppStatus>.to<MockAppStatus>(),
      bind<coriander::base::ILogger>.to<testing::mock::MockLogger>(),
      bind<coriander::os::ISystick>.to<testing::mock::MockSystick>(),
      bind<coriander::os::IThread>.to<testing::mock::MockThread>(),
      bind<coriander::os::IMutex>.to<coriander::os::posix::Mutex>(),
      bind<coriander::os::ISemaphore>.to<coriander::os::posix::Semaphore>(),
      bind<coriander::motorctl::IEncoder>.to<testing::mock::MockEncoder>(),
      bind<coriander::motorctl::IPhaseCurrentEstimator>.to<Mpce>(),
      bind<coriander::motorctl::FocMotorDriver>.to<MockFocMotorDriver>());
}
}  // namespace

TEST(BoardStateRun, basic) {
  using ParamId = coriander::base::ParamId;
  using Property = coriander::base::Property;
  using AppStatus = coriander::application::IAppStatus::Status;
  auto c = coriander::coriander_create_injector(createInjector());

  auto param = c.create<std::shared_ptr<coriander::Parameter>>();
  param->add(Property{1, ParamId::MotorCtl_General_Mode_RT});
  param->add(Property{1.0f, ParamId::MotorCtl_SpeedCtl_PidP});
  param->add(Property{0.0f, ParamId::MotorCtl_SpeedCtl_PidI});
  param->add(Property{0.0f, ParamId::MotorCtl_SpeedCtl_PidD});
  param->add(Property{99999.0f, ParamId::MotorCtl_SpeedCtl_PidOutputRamp});
  param->add(Property{99999.0f, ParamId::MotorCtl_SpeedCtl_PidLimit});
  param->add(Property{1000u, ParamId::MotorCtl_SpeedCtl_Freq});
  param->add(Property{600.0f, ParamId::MotorCtl_General_TargetVelocity_RT});
  param->add(Property{16.0f, ParamId::MotorCtl_MotorDriver_SupplyVoltage});
  param->add(Property(16, ParamId::MotorCtl_SpeedEstimator_WindowSize));
  param->add(Property{3500, ParamId::MotorCtl_SpeedEstimator_MinDuration});
  param->add(Property{50, ParamId::MotorCtl_SpeedEstimator_SampleInterval});
  param->add(Property{4, ParamId::MotorCtl_MotorDriver_PolePair});
  param->add(Property{
      static_cast<int>(coriander::motorctl::DynamicMotorCtl::Mode::Velocity),
      ParamId::MotorCtl_General_Mode_RT});
  param->add(Property{1.0f, ParamId::MotorCtl_CurrCtl_PidP});
  param->add(Property{1000.0f, ParamId::MotorCtl_CurrCtl_PidI});
  param->add(Property{0.0f, ParamId::MotorCtl_CurrCtl_PidD});
  param->add(Property{0.0f, ParamId::MotorCtl_CurrCtl_PidOutputRamp});
  param->add(Property{10.0f, ParamId::MotorCtl_CurrCtl_PidLimit});
  param->add(Property{1000u, ParamId::MotorCtl_CurrCtl_Freq});
  param->add(Property{1.0f, ParamId::MotorCtl_General_TargetCurrentD_RT});
  param->add(Property{1.0f, ParamId::MotorCtl_General_TargetCurrentQ_RT});
  param->add(Property{1e-3f, ParamId::MotorCtl_CurrCtl_Lpf_TimeConstant});
  param->add(Property{10e-3f, ParamId::MotorCtl_SpeedCtl_Lpf_TimeConstant});
  param->add(Property{100e-3f, ParamId::MotorCtl_PosCtl_Lpf_TimeConstant});

  auto encoder = c.create<std::shared_ptr<testing::mock::MockEncoder>>();

  auto runHandler =
      c.create<std::shared_ptr<coriander::IBoardStateRunHandler>>();

  runHandler->onEnter();
  runHandler->onLoop();
}
