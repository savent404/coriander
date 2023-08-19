/**
 * @file ft_diagnosis.cc
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief 
 * @date 2023-08-19
 * 
 * Copyright 2023 savent_gate
 * 
 */
#include <gtest/gtest.h>

#include "coriander/board_state.h"
#include "coriander/coriander.h"
#include "coriander/motorctl/foc_motor_driver.h"
#include "tests/mocks.h"
#include "posix/posix_appstatus.h"
#include "posix/posix_logger.h"
#include "posix/posix_mutex.h"

TEST(Coriander, DiagError) {
  using boost::di::bind;
  using testing::mock::MockBldcDriver;
  using testing::mock::MockFocMotorDriver;
  using BackendAppStatus = coriander::application::posix::AppStatus;
  auto injector = coriander::coriander_create_injector(boost::di::make_injector(
      bind<coriander::base::ILogger>.to<coriander::base::posix::Logger>(),
      bind<coriander::application::IAppStatus>.to<BackendAppStatus>(),
      bind<coriander::motorctl::IEncoder>.to<testing::mock::MockEncoder>(),
      bind<coriander::os::ISystick>.to<testing::mock::MockSystick>(),
      bind<coriander::motorctl::IBldcDriver>.to<MockBldcDriver>(),
      bind<coriander::motorctl::FocMotorDriver>.to<MockFocMotorDriver>(),
      bind<coriander::os::ISemaphore>.to<testing::mock::MockSemaphore>(),
      bind<coriander::os::IThread>.to<testing::mock::MockThread>(),
      bind<coriander::os::IMutex>.to<coriander::os::posix::Mutex>(),
      bind<coriander::IShellCtl>.to<testing::mock::MockShellCtl>()));

  auto dignosis = injector.template create<
      std::shared_ptr<coriander::application::Diagnosis>>();

  dignosis->setDiagDev(
      coriander::application::Diagnosis::DiagDevId::CtrlBoardTemp,
      []() -> coriander::application::Diagnosis::DeviceStatus {
        return coriander::application::Diagnosis::DeviceStatus::Error;
      });
  auto boardState = injector.create<std::shared_ptr<coriander::BoardState>>();

  boardState->loop();

  auto appStatus =
      injector.create<std::shared_ptr<coriander::application::IAppStatus>>();
  EXPECT_EQ(appStatus->getStatus(),
            coriander::application::IAppStatus::Status::Error);
}
