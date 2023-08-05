/**
 * @file ut_isensor.cc
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-03
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "coriander/motorctl/isensor.h"

namespace {
struct DummySensor : public coriander::motorctl::ISensor {
  MOCK_METHOD0(enable, void());
  MOCK_METHOD0(disable, void());
  MOCK_METHOD0(enabled, bool());
  MOCK_METHOD0(sync, void());
  MOCK_METHOD0(calibrate, void());
  MOCK_METHOD0(needCalibrate, bool());
};
}  // namespace

TEST(ISensor, dummy) {
  DummySensor dummy_;
  coriander::motorctl::ISensor* sensor = &dummy_;

  EXPECT_CALL(dummy_, needCalibrate())
      .Times(2)
      .WillOnce(testing::Return(true))
      .WillOnce(testing::Return(false));
  EXPECT_CALL(dummy_, calibrate()).Times(1);
  EXPECT_CALL(dummy_, sync()).Times(1);
  ASSERT_EQ(sensor->needCalibrate(), true);
  sensor->calibrate();
  ASSERT_EQ(sensor->needCalibrate(), false);

  ASSERT_TRUE(sensor->needSync(1));
  ASSERT_FALSE(sensor->needSync(1));
  ASSERT_TRUE(sensor->needSync(2));

  sensor->sync();
}
