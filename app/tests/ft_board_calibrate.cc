/**
 * @file ft_board_calibrate.cc
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-05
 *
 * Copyright 2023 savent_gate
 *
 */
#include <gtest/gtest.h>

#include <boost/di.hpp>

#include "coriander/board_event.h"
#include "coriander/board_state.h"
#include "coriander/board_state_calibrate_handler.h"
#include "coriander/iboard_event.h"
#include "coriander/iboard_state_calibrate_handler.h"
#include "coriander/iboard_state_init_handler.h"
#include "coriander/istate_handler.h"
#include "coriander/motorctl/ibldc_driver.h"
#include "coriander/motorctl/ielec_angle_estimator.h"
#include "coriander/os/isystick.h"
#include "posix/posix_mutex.h"
#include "tests/mocks.h"

namespace {
static inline auto createInjector() {
  using boost::di::bind;
  return boost::di::make_injector(
      bind<coriander::os::ISystick>().to<testing::mock::MockSystick>(),
      bind<coriander::application::IAppStatus>()
          .to<testing::mock::MockAppStatus>(),
      bind<coriander::motorctl::IBldcDriver>()
          .to<testing::mock::MockBldcDriver>(),
      bind<coriander::motorctl::IElecAngleEstimator>()
          .to<testing::mock::MockElecAngleEstimator>(),
      bind<coriander::IBoardEvent>().to<coriander::BoardEvent>(),
      bind<coriander::base::ILogger>().to<testing::mock::MockLogger>(),
      bind<coriander::IBoardState>().to<coriander::BoardState>(),
      bind<coriander::IBoardStateInitHandler>()
          .to<testing::mock::MockBoardStateInitHandler>(),
      bind<coriander::IBoardStateErrorHandler>()
          .to<testing::mock::MockBoardStateErrorHandler>(),
      bind<coriander::IBoardStateFirmwareUpdateHandler>()
          .to<testing::mock::MockBoardStateFirmwareUpdateHandler>(),
      bind<coriander::IBoardStateCalibrateHandler>()
          .to<coriander::BoardStateCalibrateHandler>(),
      bind<coriander::IBoardStateRebootHandler>()
          .to<testing::mock::MockBoardStateRebootHandler>(),
      bind<coriander::IBoardStateRunHandler>()
          .to<testing::mock::MockBoardStateRunHandler>(),
      bind<coriander::IBoardStateStandbyHandler>()
          .to<testing::mock::MockBoardStateStandbyHandler>(),
      bind<coriander::os::ISemaphore>().to<testing::mock::MockSemaphore>(),
      bind<coriander::os::IMutex>().to<coriander::os::posix::Mutex>(),
      bind<coriander::os::IThread>().to<testing::mock::MockThread>(),
      bind<coriander::Parameter>().to<testing::mock::MockPersistentParameter>(),
      bind<coriander::IParamReqValidator>()
          .to<testing::mock::MockParamReqValidator>());
}
}  // namespace

TEST(BoardStateCalibrateHandler, Calibrate) {
  using Property = coriander::base::Property;
  auto c = createInjector();

  auto event = c.create<std::shared_ptr<coriander::IBoardEvent>>();
  auto initBoardState =
      c.create<std::shared_ptr<testing::mock::MockBoardStateInitHandler>>();
  auto standbyBoardState =
      c.create<std::shared_ptr<testing::mock::MockBoardStateStandbyHandler>>();
  auto motor = c.create<std::shared_ptr<testing::mock::MockBldcDriver>>();
  auto param = c.create<std::shared_ptr<coriander::Parameter>>();
  auto calibrateBoardState =
      c.create<std::shared_ptr<coriander::IBoardStateCalibrateHandler>>();
  auto logger = c.create<std::shared_ptr<testing::mock::MockLogger>>();
  auto systick = c.create<std::shared_ptr<testing::mock::MockSystick>>();
  auto elecAngleEstimator =
      c.create<std::shared_ptr<testing::mock::MockElecAngleEstimator>>();

  // init required parameters
  using ParamId = coriander::base::ParamId;
  param->add(Property{5.0f, ParamId::MotorCtl_Calibrate_CaliVoltage});
  param->add(Property{3000, ParamId::MotorCtl_Calibrate_CaliDuration});
  param->add(Property{10.0f, ParamId::MotorCtl_MotorDriver_SupplyVoltage});

  auto board = c.create<std::shared_ptr<coriander::IBoardState>>();
  event->raiseEvent(coriander::IBoardEvent::Event::BoardInited);

  EXPECT_CALL(*standbyBoardState, onExit()).Times(1);
  EXPECT_CALL(*motor, enable()).Times(1);
  EXPECT_CALL(*motor, setPhaseDutyCycle(UINT16_MAX / 2, 0, 0)).Times(1);
  EXPECT_CALL(*systick, systick_ms()).Times(1).WillOnce(testing::Return(0));
  EXPECT_CALL(*elecAngleEstimator, enable()).Times(1);
  event->raiseEvent(coriander::IBoardEvent::Event::CalibrateStart);

  EXPECT_CALL(*systick, systick_ms()).Times(1).WillOnce(testing::Return(3001));
  EXPECT_CALL(*motor, disable()).Times(1);
  EXPECT_CALL(*standbyBoardState, onEnter()).Times(1);
  EXPECT_CALL(*elecAngleEstimator, enabled())
      .Times(1)
      .WillOnce(testing::Return(true));
  EXPECT_CALL(*elecAngleEstimator, disable()).Times(1);
  board->loop();
}
