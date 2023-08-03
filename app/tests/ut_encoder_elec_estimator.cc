/**
 * @file ut_encoder_elec_estimator.cc
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-03
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <gtest/gtest.h>

#include "coriander/motorctl/encoder_elec_angle_estimator.h"

using namespace coriander;
using namespace coriander::motorctl;

namespace {
struct dummyEncoder : public IEncoder {
  virtual void enable() { mEnabled = true; }
  virtual void disable() { mEnabled = false; }
  virtual bool enabled() { return mEnabled; }
  virtual bool needCalibrate() { return false; }
  virtual void calibrate() {}
  virtual void sync() {}
  virtual unsigned getEncoderCount() { return mCC; }
  virtual unsigned getEncoderCountPerRound() { return mPeriod; }
  virtual int getOverflowCount() { return 0; }

  unsigned mCC = 0;
  unsigned mPeriod = 4096;
  bool mEnabled = false;
};
}  // namespace

TEST(ISensor, CalibrateEncoderElecAngleEstimator) {
  ::dummyEncoder encoder;
  ParameterBase param;

  EncoderElecAngleEstimator estimator(&encoder, &param);

  param.add(Property{4, "pole_pair"});
  ASSERT_TRUE(param.has("pole_pair"_hash));

  encoder.mCC = 1024;
  estimator.enable();
  ASSERT_NEAR(estimator.getElectricalAngle(), 359, 1e-6f);
  ASSERT_TRUE(estimator.needCalibrate());
  estimator.calibrate();  // calibrate this postion as 0
  ASSERT_FALSE(estimator.needCalibrate());
  ASSERT_NEAR(estimator.getElectricalAngle(), 0.0f, 1e-6f);

  encoder.mCC += 512;
  estimator.sync();
  ASSERT_NEAR(estimator.getElectricalAngle(), 180, 1e-6f);
  estimator.disable();
}

TEST(ISensor, CalibrateEncoderElecAngleEstimatorWithPersist) {
  dummyEncoder encoder;
  ParameterBase param;

  EncoderElecAngleEstimator estimator(&encoder, &param);

  param.add(Property{4, "pole_pair"});
  param.add(Property{0.0f, "persist_raw_elec_angle"});

  encoder.mCC = 1024;
  estimator.enable();
  ASSERT_NEAR(estimator.getElectricalAngle(), 359, 1e-6f);
  ASSERT_TRUE(estimator.needCalibrate());
  estimator.calibrate();  // calibrate this postion as 0
  ASSERT_FALSE(estimator.needCalibrate());
  ASSERT_NEAR(estimator.getElectricalAngle(), 0.0f, 1e-6f);

  encoder.mCC += 512;
  estimator.sync();
  ASSERT_NEAR(estimator.getElectricalAngle(), 180, 1e-6f);
  estimator.disable();

  // re-enable to check out if any changes
  estimator.enable();
  estimator.sync();
  ASSERT_NEAR(estimator.getElectricalAngle(), 180, 1e-6f);
}

TEST(ISensor, CalibratedEncoderElecAngleEstimator) {
  dummyEncoder encoder;
  ParameterBase param;

  EncoderElecAngleEstimator estimator(&encoder, &param);

  param.add(Property{4, "pole_pair"});
  param.add(Property{0.0f, "elec_angle_offset"});

  encoder.mCC = 512;
  estimator.enable();
  ASSERT_NEAR(estimator.getElectricalAngle(), 180, 1e-6f);
  ASSERT_FALSE(estimator.needCalibrate());
  estimator.sync();
  ASSERT_NEAR(estimator.getElectricalAngle(), 180, 1e-6f);
  estimator.calibrate();
  ASSERT_NEAR(estimator.getElectricalAngle(), 0.0f, 1e-6f);
  estimator.disable();
}

TEST(ISensor, CalibratedEncoderElecAngleEstimatorWithPersist) {
  dummyEncoder encoder;
  ParameterBase param;

  EncoderElecAngleEstimator estimator(&encoder, &param);

  param.add(Property{4, "pole_pair"});
  param.add(Property{10.0f, "elec_angle_offset"});
  param.add(Property{180.0f, "persist_raw_elec_angle"});

  encoder.mCC = 512;
  estimator.enable();
  ASSERT_NEAR(estimator.getElectricalAngle(), 180 + 10, 1e-6f);
  ASSERT_FALSE(estimator.needCalibrate());
  estimator.sync();
  ASSERT_NEAR(estimator.getElectricalAngle(), 180 + 10, 1e-6f);
  estimator.calibrate();
  ASSERT_NEAR(estimator.getElectricalAngle(), 0.0f, 1e-6f);
  estimator.disable();

  // re-enable to check out if any changes
  estimator.enable();
  estimator.sync();
  ASSERT_NEAR(estimator.getElectricalAngle(), 0.0f, 1e-6f);
}

TEST(ISensor, EncoderElecAngleEstimatorRecoveryAfterPowerOn) {
  dummyEncoder encoder;
  ParameterBase param;

  EncoderElecAngleEstimator estimator(&encoder, &param);

  param.add(Property{4, "pole_pair"});
  param.add(Property{0.0f, "elec_angle_offset"});
  param.add(Property{0.0f, "persist_raw_elec_angle"});

  encoder.mCC = 0;
  estimator.enable();
  estimator.sync();
  ASSERT_NEAR(estimator.getElectricalAngle(), 0, 1e-6f);

  encoder.mCC = 512;
  estimator.sync();
  ASSERT_NEAR(estimator.getElectricalAngle(), 180, 1e-6f);
  estimator.disable();

  // assume there is a power off and param is written to flash
  encoder.mCC = 0;
  estimator.enable();
  ASSERT_NEAR(estimator.getElectricalAngle(), 180, 1e-6f);
}
