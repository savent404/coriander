#include <gtest/gtest.h>

#include "coriander/board_state.h"
#include "coriander/coriander.h"
#include "coriander/motorctl/foc_motor_driver.h"
#include "mocks.h"
#include "posix_appstatus.h"
#include "posix_logger.h"

TEST(Coriander, DiagError) {
  auto injector = coriander::coriander_create_injector(boost::di::make_injector(
      boost::di::bind<coriander::base::ILogger>.to<coriander::base::posix::Logger>(),
      boost::di::bind<coriander::application::IAppStatus>.to<coriander::application::posix::AppStatus>(),
      boost::di::bind<coriander::motorctl::IEncoder>.to<testing::mock::MockEncoder>(),
      boost::di::bind<coriander::os::ISystick>.to<testing::mock::MockSystick>(),
      boost::di::bind<coriander::motorctl::IBldcDriver>.to<testing::mock::MockBldcDriver>(),
      boost::di::bind<coriander::motorctl::FocMotorDriver>.to<testing::mock::MockFocMotorDriver>(),
      boost::di::bind<coriander::os::ISemaphore>.to<testing::mock::MockSemaphore>(),
      boost::di::bind<coriander::os::IThread>.to<testing::mock::MockThread>()));

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