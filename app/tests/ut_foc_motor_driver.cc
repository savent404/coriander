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
#include "mocks.h"

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
    ASSERT_NEAR(u, expected_u, precision);
    ASSERT_NEAR(v, expected_v, precision);
    ASSERT_NEAR(w, expected_w, precision);
  }

 public:
  uint16_t expected_u, expected_v, expected_w;
};

static auto createInjector() {
  return boost::di::make_injector(
      boost::di::bind<coriander::motorctl::IElecAngleEstimator>.to<testing::mock::MockElecAngleEstimator>(),
      boost::di::bind<coriander::motorctl::FocMotorDriver>.to<MockFocMotor>());
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
   * idx: 00, angle: 000 ,alpha: 1.00, beta: 0.00, i: 1.0000000000, j: 0.0000000000, k: 0.5000000000
   * idx: 01, angle: 010 ,alpha: 0.98, beta: 0.17, i: 0.9924038765, j: 0.0075961235, k: 0.3496162668
   * idx: 02, angle: 020 ,alpha: 0.94, beta: 0.34, i: 0.9698463104, j: 0.0301536896, k: 0.2038018673
   * idx: 03, angle: 030 ,alpha: 0.87, beta: 0.50, i: 0.9330127019, j: 0.0669872981, k: 0.0669872981
   * idx: 04, angle: 040 ,alpha: 0.77, beta: 0.64, i: 0.9185310629, j: 0.2551171147, k: 0.0814689371
   * idx: 05, angle: 050 ,alpha: 0.64, beta: 0.77, i: 0.9493452713, j: 0.3926748720, k: 0.0506547287
   * idx: 06, angle: 060 ,alpha: 0.50, beta: 0.87, i: 0.9665063509, j: 0.5334936491, k: 0.0334936491
   * idx: 07, angle: 070 ,alpha: 0.34, beta: 0.94, i: 0.9694928712, j: 0.6732947385, k: 0.0305071288
   * idx: 08, angle: 080 ,alpha: 0.17, beta: 0.98, i: 0.9582140881, j: 0.8078303550, k: 0.0417859119
   * idx: 09, angle: 090 ,alpha: 0.00, beta: 1.00, i: 0.9330127019, j: 0.9330127019, k: 0.0669872981
   * idx: 10, angle: 100 ,alpha: -0.17, beta: 0.98, i: 0.8078303550, j: 0.9582140881, k: 0.0417859119
   * idx: 11, angle: 110 ,alpha: -0.34, beta: 0.94, i: 0.6732947385, j: 0.9694928712, k: 0.0305071288
   * idx: 12, angle: 120 ,alpha: -0.50, beta: 0.87, i: 0.5334936491, j: 0.9665063509, k: 0.0334936491
   * idx: 13, angle: 130 ,alpha: -0.64, beta: 0.77, i: 0.3926748720, j: 0.9493452713, k: 0.0506547287
   * idx: 14, angle: 140 ,alpha: -0.77, beta: 0.64, i: 0.2551171147, j: 0.9185310629, k: 0.0814689371
   * idx: 15, angle: 150 ,alpha: -0.87, beta: 0.50, i: 0.1250000000, j: 0.8750000000, k: 0.1250000000
   * idx: 16, angle: 160 ,alpha: -0.94, beta: 0.34, i: 0.0301536896, j: 0.9698463104, k: 0.2038018673
   * idx: 17, angle: 170 ,alpha: -0.98, beta: 0.17, i: 0.0075961235, j: 0.9924038765, k: 0.3496162668
   * idx: 18, angle: 180 ,alpha: -1.00, beta: 0.00, i: 0.0000000000, j: 1.0000000000, k: 0.5000000000
   * idx: 19, angle: 190 ,alpha: -0.98, beta: -0.17, i: 0.0075961235, j: 0.9924038765, k: 0.6503837332
   * idx: 20, angle: 200 ,alpha: -0.94, beta: -0.34, i: 0.0301536896, j: 0.9698463104, k: 0.7961981327
   * idx: 21, angle: 210 ,alpha: -0.87, beta: -0.50, i: 0.1250000000, j: 0.8750000000, k: 0.8750000000
   * idx: 22, angle: 220 ,alpha: -0.77, beta: -0.64, i: 0.0814689371, j: 0.7448828853, k: 0.9185310629
   * idx: 23, angle: 230 ,alpha: -0.64, beta: -0.77, i: 0.0506547287, j: 0.6073251280, k: 0.9493452713
   * idx: 24, angle: 240 ,alpha: -0.50, beta: -0.87, i: 0.0334936491, j: 0.4665063509, k: 0.9665063509
   * idx: 25, angle: 250 ,alpha: -0.34, beta: -0.94, i: 0.0305071288, j: 0.3267052615, k: 0.9694928712
   * idx: 26, angle: 260 ,alpha: -0.17, beta: -0.98, i: 0.0417859119, j: 0.1921696450, k: 0.9582140881
   * idx: 27, angle: 270 ,alpha: -0.00, beta: -1.00, i: 0.0669872981, j: 0.0669872981, k: 0.9330127019
   * idx: 28, angle: 280 ,alpha: 0.17, beta: -0.98, i: 0.1921696450, j: 0.0417859119, k: 0.9582140881
   * idx: 29, angle: 290 ,alpha: 0.34, beta: -0.94, i: 0.3267052615, j: 0.0305071288, k: 0.9694928712
   * idx: 30, angle: 300 ,alpha: 0.50, beta: -0.87, i: 0.4665063509, j: 0.0334936491, k: 0.9665063509
   * idx: 31, angle: 310 ,alpha: 0.64, beta: -0.77, i: 0.6073251280, j: 0.0506547287, k: 0.9493452713
   * idx: 32, angle: 320 ,alpha: 0.77, beta: -0.64, i: 0.7448828853, j: 0.0814689371, k: 0.9185310629
   * idx: 33, angle: 330 ,alpha: 0.87, beta: -0.50, i: 0.9330127019, j: 0.0669872981, k: 0.9330127019
   * idx: 34, angle: 340 ,alpha: 0.94, beta: -0.34, i: 0.9698463104, j: 0.0301536896, k: 0.7961981327
   * idx: 35, angle: 350 ,alpha: 0.98, beta: -0.17, i: 0.9924038765, j: 0.0075961235, k: 0.6503837332
  */
  // clang-format on

  float i[] = {1.0000000000, 0.9924038765, 0.9698463104, 0.9330127019,
               0.9185310629, 0.9493452713, 0.9665063509, 0.9694928712,
               0.9582140881, 0.9330127019, 0.8078303550, 0.6732947385,
               0.5334936491, 0.3926748720, 0.2551171147, 0.1250000000,
               0.0301536896, 0.0075961235, 0.0000000000, 0.0075961235,
               0.0301536896, 0.1250000000, 0.0814689371, 0.0506547287,
               0.0334936491, 0.0305071288, 0.0417859119, 0.0669872981,
               0.1921696450, 0.3267052615, 0.4665063509, 0.6073251280,
               0.7448828853, 0.9330127019, 0.9698463104, 0.9924038765};
  float j[] = {0.0000000000, 0.0075961235, 0.0301536896, 0.0669872981,
               0.2551171147, 0.3926748720, 0.5334936491, 0.6732947385,
               0.8078303550, 0.9330127019, 0.9582140881, 0.9694928712,
               0.9665063509, 0.9493452713, 0.9185310629, 0.8750000000,
               0.9698463104, 0.9924038765, 1.0000000000, 0.9924038765,
               0.9698463104, 0.8750000000, 0.7448828853, 0.6073251280,
               0.4665063509, 0.3267052615, 0.1921696450, 0.0669872981,
               0.0417859119, 0.0305071288, 0.0334936491, 0.0506547287,
               0.0814689371, 0.0669872981, 0.0301536896, 0.0075961235};
  float k[] = {0.5000000000, 0.3496162668, 0.2038018673, 0.0669872981,
               0.0814689371, 0.0506547287, 0.0334936491, 0.0305071288,
               0.0417859119, 0.0669872981, 0.0417859119, 0.0305071288,
               0.0334936491, 0.0506547287, 0.0814689371, 0.1250000000,
               0.2038018673, 0.3496162668, 0.5000000000, 0.6503837332,
               0.7961981327, 0.8750000000, 0.9185310629, 0.9493452713,
               0.9665063509, 0.9694928712, 0.9582140881, 0.9330127019,
               0.9582140881, 0.9694928712, 0.9665063509, 0.9493452713,
               0.9185310629, 0.8750000000, 0.7961981327, 0.6503837332};

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
