/**
 * @file ut_motor_ctl_current.cc
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-21
 *
 * Copyright 2023 savent_gate
 *
 */
#include <gtest/gtest.h>

#include "boost/di.hpp"
#include "coriander/motorctl/iphase_current_estimator.h"
#include "coriander/motorctl/motor_ctl_current.h"
#include "coriander/parameters.h"
#include "tests/mocks.h"

namespace {
auto& createInjector() {
  using boost::di::bind;
  using testing::mock::MockElecAngleEstimator;
  using testing::mock::MockParamReqValidator;
  using testing::mock::MockPhaseCurrentEstimator;
  using testing::mock::MockSystick;
  using Mfmd = testing::mock::MockFocMotorDriver;
  using Mpce = testing::mock::MockPhaseCurrentEstimator;
  using Meae = testing::mock::MockElecAngleEstimator;
  static auto injector = boost::di::make_injector(
      bind<coriander::IParamReqValidator>.to<MockParamReqValidator>(),
      bind<coriander::os::ISystick>.to<MockSystick>(),
      bind<coriander::motorctl::FocMotorDriver>.to<Mfmd>(),
      bind<coriander::motorctl::IPhaseCurrentEstimator>.to<Mpce>(),
      bind<coriander::Parameter>.to<testing::mock::MockPersistentParameter>(),
      bind<coriander::motorctl::IElecAngleEstimator>.to<Meae>());
  return injector;
}
}  // namespace

TEST(MotorCtlCurrent, targetMode) {
  using ID = coriander::base::ParamId;
  using coriander::base::Property;
  using testing::_;
  auto& injector = createInjector();
  auto param = injector.create<std::shared_ptr<coriander::Parameter>>();
  auto systick = injector.create<std::shared_ptr<testing::mock::MockSystick>>();
  auto motor =
      injector.create<std::shared_ptr<testing::mock::MockFocMotorDriver>>();
  auto currentSensor =
      injector
          .create<std::shared_ptr<testing::mock::MockPhaseCurrentEstimator>>();

  param->add(Property{1.0f, ID::MotorCtl_CurrCtl_PidP});
  param->add(Property{1000.0f, ID::MotorCtl_CurrCtl_PidI});
  param->add(Property{0.0f, ID::MotorCtl_CurrCtl_PidD});
  param->add(Property{0.0f, ID::MotorCtl_CurrCtl_PidOutputRamp});
  param->add(Property{10.0f, ID::MotorCtl_CurrCtl_PidLimit});
  param->add(Property{1000u, ID::MotorCtl_CurrCtl_Freq});
  param->add(Property{1.0f, ID::MotorCtl_General_TargetCurrentD_RT});
  param->add(Property{1.0f, ID::MotorCtl_General_TargetCurrentQ_RT});
  param->add(Property{0.0f, ID::MotorCtl_CurrCtl_Lpf_TimeConstant});

  auto motorCtl =
      injector.create<std::shared_ptr<coriander::motorctl::MotorCtlCurrent>>();

  EXPECT_CALL(*systick, systick_us()).WillRepeatedly(testing::Return(0));
  EXPECT_CALL(*currentSensor, enable()).Times(1);
  EXPECT_CALL(*motor, enable()).Times(1);
  motorCtl->start();

  EXPECT_CALL(*currentSensor, sync()).Times(1);
  ON_CALL(*currentSensor, getPhaseCurrent(_, _))
      .WillByDefault(testing::DoAll(testing::SetArgPointee<0>(0.0f),
                                    testing::SetArgPointee<1>(0.0f)));
  EXPECT_CALL(*systick, systick_us()).WillRepeatedly(testing::Return(2000));
  EXPECT_CALL(*motor, setVoltage(1.5f, 1.5f)).Times(1);
  motorCtl->loop();

  EXPECT_CALL(*currentSensor, disable()).Times(1);
  EXPECT_CALL(*motor, disable()).Times(1);
  motorCtl->stop();
}
