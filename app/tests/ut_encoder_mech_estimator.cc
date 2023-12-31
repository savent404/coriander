/**
 * @file ut_encoder_mech_estimator.cc
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-03
 *
 * Copyright 2023 savent_gate
 *
 */
#include <gtest/gtest.h>

#include "coriander/motorctl/encoder_mech_angle_estimator.h"
#include "coriander/motorctl/iencoder.h"
#include "coriander/parameters.h"
#include "tests/mocks.h"

using ParamId = coriander::base::ParamId;
using Parameter = testing::mock::MockPersistentParameter;
using coriander::base::Property;
using coriander::motorctl::EncoderMechAngleEstimator;
using coriander::motorctl::IEncoder;
using coriander::motorctl::IMechAngleEstimator;

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
  virtual int getOverflowCount() { return mOverflow; }

  unsigned mCC = 0;
  int mOverflow = 0;
  unsigned mPeriod = 4096;
  bool mEnabled = false;
};
}  // namespace

TEST(ISensor, BasicEncoderMechAngleEstimator) {
  auto encoder = std::make_shared<::dummyEncoder>();
  auto param = std::make_shared<Parameter>();
  auto paramReqValidator =
      std::make_shared<testing::mock::MockParamReqValidator>();

  EncoderMechAngleEstimator estimator(encoder, param, paramReqValidator);

  encoder->mCC = 0;
  encoder->mOverflow = 2;
  encoder->mPeriod = 4096;
  encoder->mEnabled = false;

  estimator.enable();
  estimator.sync();
  ASSERT_NEAR(estimator.getMechanicalAngle(), 720.0f, 1e-6f);
}

TEST(ISensor, CalibrateEncoderMechAngleEstimator) {
  std::shared_ptr<::dummyEncoder> encoder = std::make_shared<::dummyEncoder>();
  std::shared_ptr<Parameter> param = std::make_shared<Parameter>();
  auto paramReqValidator =
      std::make_shared<testing::mock::MockParamReqValidator>();

  EncoderMechAngleEstimator estimator(encoder, param, paramReqValidator);

  param->add(Property{0.0f, ParamId::MotorCtl_Calibrate_CaliMechAngleOffset});

  encoder->mOverflow = 1;

  estimator.enable();
  estimator.sync();
  ASSERT_NEAR(estimator.getMechanicalAngle(), 360.0f, 1e-6f);

  estimator.calibrate();
  ASSERT_NEAR(estimator.getMechanicalAngle(), 0.0f, 1e-6f);
  estimator.disable();
}

TEST(ISensor, PersistEncoderMechAngleEstimator) {
  auto encoder = std::make_shared<::dummyEncoder>();
  auto param = std::make_shared<Parameter>();
  auto paramReqValidator =
      std::make_shared<testing::mock::MockParamReqValidator>();

  EncoderMechAngleEstimator estimator(encoder, param, paramReqValidator);

  param->add(Property{0.0f, ParamId::MotorCtl_Calibrate_CaliMechAngleOffset});
  param->add(Property{0.0f, ParamId::MotorCtl_MotorDriver_PersistRawMechAngle});

  encoder->mOverflow = 1;

  estimator.enable();
  estimator.sync();
  ASSERT_NEAR(estimator.getMechanicalAngle(), 0.0f, 1e-6f);

  encoder->mOverflow += 1;
  estimator.sync();
  ASSERT_NEAR(estimator.getMechanicalAngle(), 360.0f, 1e-6);

  estimator.disable();

  estimator.enable();
  estimator.sync();
  ASSERT_NEAR(estimator.getMechanicalAngle(), 360.0f, 1e-6f);
}
