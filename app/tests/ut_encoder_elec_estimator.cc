/**
 * @file ut_encoder_elec_estimator.cc
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-03
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <gmock/gmock.h>
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
  MOCK_METHOD0(calibrate, void());
  MOCK_METHOD0(sync, void());
  virtual unsigned getEncoderCount() { return mCC; }
  virtual unsigned getEncoderCountPerRound() { return 4096; }
  MOCK_METHOD0(getOverflowCount, int());

  unsigned mCC = 0;
  bool mEnabled = false;
};
}  // namespace

TEST(ISensor, CalibrateEncoderElecAngleEstimator) {
  auto encoder = std::make_shared<::dummyEncoder>();
  auto param = std::make_shared<ParameterBase>();

  EncoderElecAngleEstimator estimator(encoder, param);

  param->add(Property{4, "pole_pair"});
  ASSERT_TRUE(param->has("pole_pair"_hash));

  encoder->mCC = 1024;
  estimator.enable();
  ASSERT_NEAR(estimator.getElectricalAngle(), 359, 1e-6f);
  ASSERT_TRUE(estimator.needCalibrate());
  estimator.calibrate();  // calibrate this postion as 0
  ASSERT_FALSE(estimator.needCalibrate());
  ASSERT_NEAR(estimator.getElectricalAngle(), 0.0f, 1e-6f);

  encoder->mCC += 512;
  estimator.sync();
  ASSERT_NEAR(estimator.getElectricalAngle(), 180, 1e-6f);
  estimator.disable();
}

TEST(ISensor, CalibrateEncoderElecAngleEstimatorWithPersist) {
  auto encoder = std::make_shared<::dummyEncoder>();
  auto param = std::make_shared<ParameterBase>();

  EncoderElecAngleEstimator estimator(encoder, param);

  param->add(Property{4, "pole_pair"});
  param->add(Property{0.0f, "persist_raw_elec_angle"});

  encoder->mCC = 1024;
  estimator.enable();
  ASSERT_NEAR(estimator.getElectricalAngle(), 359, 1e-6f);
  ASSERT_TRUE(estimator.needCalibrate());
  estimator.calibrate();  // calibrate this postion as 0
  ASSERT_FALSE(estimator.needCalibrate());
  ASSERT_NEAR(estimator.getElectricalAngle(), 0.0f, 1e-6f);

  encoder->mCC += 512;
  estimator.sync();
  ASSERT_NEAR(estimator.getElectricalAngle(), 180, 1e-6f);
  estimator.disable();

  // re-enable to check out if any changes
  estimator.enable();
  estimator.sync();
  ASSERT_NEAR(estimator.getElectricalAngle(), 180, 1e-6f);
}

TEST(ISensor, CalibratedEncoderElecAngleEstimator) {
  auto encoder = std::make_shared<::dummyEncoder>();
  auto param = std::make_shared<ParameterBase>();

  EncoderElecAngleEstimator estimator(encoder, param);

  param->add(Property{4, "pole_pair"});
  param->add(Property{0.0f, "elec_angle_offset"});

  encoder->mCC = 512;
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
  auto encoder = std::make_shared<::dummyEncoder>();
  auto param = std::make_shared<ParameterBase>();

  EncoderElecAngleEstimator estimator(encoder, param);

  param->add(Property{4, "pole_pair"});
  param->add(Property{10.0f, "elec_angle_offset"});
  param->add(Property{180.0f, "persist_raw_elec_angle"});

  encoder->mCC = 512;
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
  auto encoder = std::make_shared<::dummyEncoder>();
  auto param = std::make_shared<ParameterBase>();

  EncoderElecAngleEstimator estimator(encoder, param);

  param->add(Property{4, "pole_pair"});
  param->add(Property{0.0f, "elec_angle_offset"});
  param->add(Property{0.0f, "persist_raw_elec_angle"});

  encoder->mCC = 0;
  estimator.enable();
  estimator.sync();
  ASSERT_NEAR(estimator.getElectricalAngle(), 0, 1e-6f);

  encoder->mCC = 512;
  estimator.sync();
  ASSERT_NEAR(estimator.getElectricalAngle(), 180, 1e-6f);
  estimator.disable();

  // assume there is a power off and param is written to flash
  encoder->mCC = 0;
  estimator.enable();
  ASSERT_NEAR(estimator.getElectricalAngle(), 180, 1e-6f);
}