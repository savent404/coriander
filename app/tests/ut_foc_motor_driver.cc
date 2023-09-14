/**
 * @file ut_foc_motor_driver.cc
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-05
 *
 * Copyright 2023 savent_gate
 *
 */
#include <gtest/gtest.h>

#include <boost/di.hpp>

#include "coriander/motorctl/foc_motor_driver.h"
#include "coriander/motorctl/ibldc_driver.h"
#include "coriander/motorctl/ielec_angle_estimator.h"
#include "tests/mocks.h"

namespace {

struct MockFocMotor : public coriander::motorctl::FocMotorDriver {
  MockFocMotor(std::shared_ptr<coriander::motorctl::IElecAngleEstimator>
                   elecAngleEstimator)
      : coriander::motorctl::FocMotorDriver(elecAngleEstimator) {}
  MOCK_METHOD0(enable, void());
  MOCK_METHOD0(disable, void());
  MOCK_METHOD0(emergencyBreak, void());
  MOCK_METHOD0(fatalError, bool());

  void setPhaseDutyCycle(uint16_t u, uint16_t v, uint16_t w) override {
    uint16_t precision = UINT16_MAX * 1e-4;
    EXPECT_NEAR(u, expected_u, precision);
    EXPECT_NEAR(v, expected_v, precision);
    EXPECT_NEAR(w, expected_w, precision);
  }

 public:
  uint16_t expected_u, expected_v, expected_w;
};

static auto createInjector() {
  using boost::di::bind;
  using coriander::motorctl::FocMotorDriver;
  using coriander::motorctl::IElecAngleEstimator;
  using testing::mock::MockElecAngleEstimator;
  return boost::di::make_injector(
      bind<IElecAngleEstimator>.to<MockElecAngleEstimator>(),
      bind<FocMotorDriver>.to<MockFocMotor>());
}

}  // namespace
TEST(FocMotorDriver, key_point) {
  auto c = createInjector();
  auto t = c.create<std::shared_ptr<MockFocMotor>>();
  auto estimator =
      c.create<std::shared_ptr<testing::mock::MockElecAngleEstimator>>();

  // expected data is generate by python script, run app/tests/svpwm_emulator.py
  // to generate.

  // clang-format off
  /**
idx: 00, angle: 000, alpha: 1.00, beta: 0.00, i: 0.8750000000, j: 0.1250000000, k: 0.1250000000
idx: 01, angle: 010, alpha: 0.98, beta: 0.17, i: 0.9068988407, j: 0.2434848925, k: 0.0931011593
idx: 02, angle: 020, alpha: 0.94, beta: 0.34, i: 0.9264342660, j: 0.3697638667, k: 0.0735657340
idx: 03, angle: 030, alpha: 0.87, beta: 0.50, i: 0.9330127019, j: 0.5000000000, k: 0.0669872981
idx: 04, angle: 040, alpha: 0.77, beta: 0.64, i: 0.9264342660, j: 0.6302361333, k: 0.0735657340
idx: 05, angle: 050, alpha: 0.64, beta: 0.77, i: 0.9068988407, j: 0.7565151075, k: 0.0931011593
idx: 06, angle: 060, alpha: 0.50, beta: 0.87, i: 0.8750000000, j: 0.8750000000, k: 0.1250000000
idx: 07, angle: 070, alpha: 0.34, beta: 0.94, i: 0.7565151075, j: 0.9068988407, k: 0.0931011593
idx: 08, angle: 080, alpha: 0.17, beta: 0.98, i: 0.6302361333, j: 0.9264342660, k: 0.0735657340
idx: 09, angle: 090, alpha: 0.00, beta: 1.00, i: 0.5000000000, j: 0.9330127019, k: 0.0669872981
idx: 10, angle: 100, alpha: -0.17, beta: 0.98, i: 0.3697638667, j: 0.9264342660, k: 0.0735657340
idx: 11, angle: 110, alpha: -0.34, beta: 0.94, i: 0.2434848925, j: 0.9068988407, k: 0.0931011593
idx: 12, angle: 120, alpha: -0.50, beta: 0.87, i: 0.1250000000, j: 0.8750000000, k: 0.1250000000
idx: 13, angle: 130, alpha: -0.64, beta: 0.77, i: 0.0931011593, j: 0.9068988407, k: 0.2434848925
idx: 14, angle: 140, alpha: -0.77, beta: 0.64, i: 0.0735657340, j: 0.9264342660, k: 0.3697638667
idx: 15, angle: 150, alpha: -0.87, beta: 0.50, i: 0.0669872981, j: 0.9330127019, k: 0.5000000000
idx: 16, angle: 160, alpha: -0.94, beta: 0.34, i: 0.0735657340, j: 0.9264342660, k: 0.6302361333
idx: 17, angle: 170, alpha: -0.98, beta: 0.17, i: 0.0931011593, j: 0.9068988407, k: 0.7565151075
idx: 18, angle: 180, alpha: -1.00, beta: 0.00, i: 0.1250000000, j: 0.8750000000, k: 0.8750000000
idx: 19, angle: 190, alpha: -0.98, beta: -0.17, i: 0.0931011593, j: 0.7565151075, k: 0.9068988407
idx: 20, angle: 200, alpha: -0.94, beta: -0.34, i: 0.0735657340, j: 0.6302361333, k: 0.9264342660
idx: 21, angle: 210, alpha: -0.87, beta: -0.50, i: 0.0669872981, j: 0.5000000000, k: 0.9330127019
idx: 22, angle: 220, alpha: -0.77, beta: -0.64, i: 0.0735657340, j: 0.3697638667, k: 0.9264342660
idx: 23, angle: 230, alpha: -0.64, beta: -0.77, i: 0.0931011593, j: 0.2434848925, k: 0.9068988407
idx: 24, angle: 240, alpha: -0.50, beta: -0.87, i: 0.1250000000, j: 0.1250000000, k: 0.8750000000
idx: 25, angle: 250, alpha: -0.34, beta: -0.94, i: 0.2434848925, j: 0.0931011593, k: 0.9068988407
idx: 26, angle: 260, alpha: -0.17, beta: -0.98, i: 0.3697638667, j: 0.0735657340, k: 0.9264342660
idx: 27, angle: 270, alpha: -0.00, beta: -1.00, i: 0.5000000000, j: 0.0669872981, k: 0.9330127019
idx: 28, angle: 280, alpha: 0.17, beta: -0.98, i: 0.6302361333, j: 0.0735657340, k: 0.9264342660
idx: 29, angle: 290, alpha: 0.34, beta: -0.94, i: 0.7565151075, j: 0.0931011593, k: 0.9068988407
idx: 30, angle: 300, alpha: 0.50, beta: -0.87, i: 0.8750000000, j: 0.1250000000, k: 0.8750000000
idx: 31, angle: 310, alpha: 0.64, beta: -0.77, i: 0.9068988407, j: 0.0931011593, k: 0.7565151075
idx: 32, angle: 320, alpha: 0.77, beta: -0.64, i: 0.9264342660, j: 0.0735657340, k: 0.6302361333
idx: 33, angle: 330, alpha: 0.87, beta: -0.50, i: 0.9330127019, j: 0.0669872981, k: 0.5000000000
idx: 34, angle: 340, alpha: 0.94, beta: -0.34, i: 0.9264342660, j: 0.0735657340, k: 0.3697638667
idx: 35, angle: 350, alpha: 0.98, beta: -0.17, i: 0.9068988407, j: 0.0931011593, k: 0.2434848925
  */
  // clang-format on

  float i[] = {0.8750000000, 0.9068988407, 0.9264342660, 0.9330127019,
               0.9264342660, 0.9068988407, 0.8750000000, 0.7565151075,
               0.6302361333, 0.5000000000, 0.3697638667, 0.2434848925,
               0.1250000000, 0.0931011593, 0.0735657340, 0.0669872981,
               0.0735657340, 0.0931011593, 0.1250000000, 0.0931011593,
               0.0735657340, 0.0669872981, 0.0735657340, 0.0931011593,
               0.1250000000, 0.2434848925, 0.3697638667, 0.5000000000,
               0.6302361333, 0.7565151075, 0.8750000000, 0.9068988407,
               0.9264342660, 0.9330127019, 0.9264342660, 0.9068988407};
  float j[] = {0.1250000000, 0.2434848925, 0.3697638667, 0.5000000000,
               0.6302361333, 0.7565151075, 0.8750000000, 0.9068988407,
               0.9264342660, 0.9330127019, 0.9264342660, 0.9068988407,
               0.8750000000, 0.9068988407, 0.9264342660, 0.9330127019,
               0.9264342660, 0.9068988407, 0.8750000000, 0.7565151075,
               0.6302361333, 0.5000000000, 0.3697638667, 0.2434848925,
               0.1250000000, 0.0931011593, 0.0735657340, 0.0669872981,
               0.0735657340, 0.0931011593, 0.1250000000, 0.0931011593,
               0.0735657340, 0.0669872981, 0.0735657340, 0.0931011593};
  float k[] = {0.1250000000, 0.0931011593, 0.0735657340, 0.0669872981,
               0.0735657340, 0.0931011593, 0.1250000000, 0.0931011593,
               0.0735657340, 0.0669872981, 0.0735657340, 0.0931011593,
               0.1250000000, 0.2434848925, 0.3697638667, 0.5000000000,
               0.6302361333, 0.7565151075, 0.8750000000, 0.9068988407,
               0.9264342660, 0.9330127019, 0.9264342660, 0.9068988407,
               0.8750000000, 0.9068988407, 0.9264342660, 0.9330127019,
               0.9264342660, 0.9068988407, 0.8750000000, 0.7565151075,
               0.6302361333, 0.5000000000, 0.3697638667, 0.2434848925};

  EXPECT_CALL(*estimator, getElectricalAngle())
      .Times(36)
      .WillOnce(testing::Return(0))
      .WillOnce(testing::Return(10))
      .WillOnce(testing::Return(20))
      .WillOnce(testing::Return(30))
      .WillOnce(testing::Return(40))
      .WillOnce(testing::Return(50))
      .WillOnce(testing::Return(60))
      .WillOnce(testing::Return(70))
      .WillOnce(testing::Return(80))
      .WillOnce(testing::Return(90))
      .WillOnce(testing::Return(100))
      .WillOnce(testing::Return(110))
      .WillOnce(testing::Return(120))
      .WillOnce(testing::Return(130))
      .WillOnce(testing::Return(140))
      .WillOnce(testing::Return(150))
      .WillOnce(testing::Return(160))
      .WillOnce(testing::Return(170))
      .WillOnce(testing::Return(180))
      .WillOnce(testing::Return(190))
      .WillOnce(testing::Return(200))
      .WillOnce(testing::Return(210))
      .WillOnce(testing::Return(220))
      .WillOnce(testing::Return(230))
      .WillOnce(testing::Return(240))
      .WillOnce(testing::Return(250))
      .WillOnce(testing::Return(260))
      .WillOnce(testing::Return(270))
      .WillOnce(testing::Return(280))
      .WillOnce(testing::Return(290))
      .WillOnce(testing::Return(300))
      .WillOnce(testing::Return(310))
      .WillOnce(testing::Return(320))
      .WillOnce(testing::Return(330))
      .WillOnce(testing::Return(340))
      .WillOnce(testing::Return(350));

  for (int idx = 0; idx < 36; idx++) {
    std::cout << "idx: " << idx << std::endl;
    t->expected_u = static_cast<uint16_t>(i[idx] * UINT16_MAX);
    t->expected_v = static_cast<uint16_t>(j[idx] * UINT16_MAX);
    t->expected_w = static_cast<uint16_t>(k[idx] * UINT16_MAX);
    t->setVoltage(1.0f, 0);
  }
}
