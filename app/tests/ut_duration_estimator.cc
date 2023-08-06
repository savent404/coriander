/**
 * @file ut_duration_estimator.cc
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-06
 *
 * Copyright 2023 savent_gate
 *
 */
#include <gtest/gtest.h>

#include "coriander/motorctl/duration_estimator.h"
#include "coriander/os/isystick.h"
#include "mocks.h"

TEST(DurationEstimator, OneShot) {
  using coriander::motorctl::DurationEstimator;
  using coriander::motorctl::DurationEstimatorType;
  using coriander::motorctl::DurationEstimatorUnit;
  using testing::mock::MockSystick;
  auto systick = std::make_shared<MockSystick>();
  DurationEstimator<DurationEstimatorType::OneShot, DurationEstimatorUnit::MS>
      duration_estimator(systick);
  EXPECT_CALL(*systick, systick_ms()).Times(1).WillOnce(testing::Return(0));
  duration_estimator.recordStart();
  EXPECT_CALL(*systick, systick_ms()).Times(1).WillOnce(testing::Return(1000));
  duration_estimator.recordStop();
  EXPECT_EQ(duration_estimator.getDuration(), 1000);

  DurationEstimator<DurationEstimatorType::OneShot, DurationEstimatorUnit::US>
      duration_estimator2(systick);
  EXPECT_CALL(*systick, systick_us()).Times(1).WillOnce(testing::Return(0));
  duration_estimator2.recordStart();
  EXPECT_CALL(*systick, systick_us()).Times(1).WillOnce(testing::Return(1000));
  duration_estimator2.recordStop();
  EXPECT_EQ(duration_estimator2.getDuration(), 1000);
}

TEST(DurationEstimator, Max) {
  using coriander::motorctl::DurationEstimator;
  using coriander::motorctl::DurationEstimatorType;
  using coriander::motorctl::DurationEstimatorUnit;
  using testing::mock::MockSystick;
  auto systick = std::make_shared<MockSystick>();
  DurationEstimator<DurationEstimatorType::Max, DurationEstimatorUnit::MS>
      duration_estimator(systick);
  EXPECT_CALL(*systick, systick_ms())
      .Times(2)
      .WillOnce(testing::Return(0))
      .WillOnce(testing::Return(1000));
  duration_estimator.recordStart();
  duration_estimator.recordStop();
  EXPECT_CALL(*systick, systick_ms())
      .Times(2)
      .WillOnce(testing::Return(1000))
      .WillOnce(testing::Return(3000));
  duration_estimator.recordStart();
  duration_estimator.recordStop();
  EXPECT_EQ(duration_estimator.getDuration(), 2000);

  DurationEstimator<DurationEstimatorType::Max, DurationEstimatorUnit::US>
      duration_estimator2(systick);
  EXPECT_CALL(*systick, systick_us())
      .Times(2)
      .WillOnce(testing::Return(0))
      .WillOnce(testing::Return(1000));
  duration_estimator2.recordStart();
  duration_estimator2.recordStop();
  EXPECT_CALL(*systick, systick_us())
      .Times(2)
      .WillOnce(testing::Return(1000))
      .WillOnce(testing::Return(3000));
  duration_estimator2.recordStart();
  duration_estimator2.recordStop();
  EXPECT_EQ(duration_estimator2.getDuration(), 2000);
}

TEST(DurationEstimator, Min) {
  using coriander::motorctl::DurationEstimator;
  using coriander::motorctl::DurationEstimatorType;
  using coriander::motorctl::DurationEstimatorUnit;
  using testing::mock::MockSystick;
  auto systick = std::make_shared<MockSystick>();
  DurationEstimator<DurationEstimatorType::Min, DurationEstimatorUnit::MS>
      duration_estimator(systick);
  EXPECT_CALL(*systick, systick_ms())
      .Times(2)
      .WillOnce(testing::Return(0))
      .WillOnce(testing::Return(1000));
  duration_estimator.recordStart();
  duration_estimator.recordStop();
  EXPECT_CALL(*systick, systick_ms())
      .Times(2)
      .WillOnce(testing::Return(1000))
      .WillOnce(testing::Return(3000));
  duration_estimator.recordStart();
  duration_estimator.recordStop();
  EXPECT_EQ(duration_estimator.getDuration(), 1000);

  DurationEstimator<DurationEstimatorType::Min, DurationEstimatorUnit::US>
      duration_estimator2(systick);
  EXPECT_CALL(*systick, systick_us())
      .Times(2)
      .WillOnce(testing::Return(0))
      .WillOnce(testing::Return(1000));
  duration_estimator2.recordStart();
  duration_estimator2.recordStop();
  EXPECT_CALL(*systick, systick_us())
      .Times(2)
      .WillOnce(testing::Return(1000))
      .WillOnce(testing::Return(3000));
  duration_estimator2.recordStart();
  duration_estimator2.recordStop();
  EXPECT_EQ(duration_estimator2.getDuration(), 1000);
}


TEST(DurationEstimator, Average) {
  using coriander::motorctl::DurationEstimator;
  using coriander::motorctl::DurationEstimatorType;
  using coriander::motorctl::DurationEstimatorUnit;
  using testing::mock::MockSystick;
  auto systick = std::make_shared<MockSystick>();
  DurationEstimator<DurationEstimatorType::Average, DurationEstimatorUnit::MS>
      duration_estimator(systick);
  EXPECT_CALL(*systick, systick_ms())
      .Times(2)
      .WillOnce(testing::Return(0))
      .WillOnce(testing::Return(1000));
  duration_estimator.recordStart();
  duration_estimator.recordStop();
  EXPECT_CALL(*systick, systick_ms())
      .Times(2)
      .WillOnce(testing::Return(1000))
      .WillOnce(testing::Return(3000));
  duration_estimator.recordStart();
  duration_estimator.recordStop();
  EXPECT_EQ(duration_estimator.getDuration(), 1500);

  DurationEstimator<DurationEstimatorType::Average, DurationEstimatorUnit::US>
      duration_estimator2(systick);
  EXPECT_CALL(*systick, systick_us())
      .Times(2)
      .WillOnce(testing::Return(0))
      .WillOnce(testing::Return(1000));
  duration_estimator2.recordStart();
  duration_estimator2.recordStop();
  EXPECT_CALL(*systick, systick_us())
      .Times(2)
      .WillOnce(testing::Return(1000))
      .WillOnce(testing::Return(3000));
  duration_estimator2.recordStart();
  duration_estimator2.recordStop();
  EXPECT_EQ(duration_estimator2.getDuration(), 1500);
}
