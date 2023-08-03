/**
 * @file ut_isensor.cc
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-03
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <gtest/gtest.h>

#include "coriander/motorctl/isensor.h"

using namespace coriander::motorctl;

struct dummySensor : public ISensor {
  virtual void enable() {}
  virtual void disable() {}
  virtual bool enabled() { return true; }
  virtual void sync() { mCnt++; }
  virtual void calibrate() { mCnt = 0; }
  virtual bool needCalibrate() { return mCnt < 0; }
  int mCnt = -1;
};
TEST(ISensor, dummy) {
  auto dummy = new dummySensor();
  ISensor* sensor = dummy;
  ASSERT_EQ(sensor->needCalibrate(), true);
  sensor->calibrate();
  ASSERT_EQ(sensor->needCalibrate(), false);
  ASSERT_EQ(dummy->mCnt, 0);

  ASSERT_TRUE(sensor->needSync(1));
  ASSERT_FALSE(sensor->needSync(1));
  ASSERT_TRUE(sensor->needSync(2));

  sensor->sync();
  ASSERT_EQ(dummy->mCnt, 1);
}
