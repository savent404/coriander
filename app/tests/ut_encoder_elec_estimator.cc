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
#include "mocks.h"

using namespace coriander;
using namespace coriander::motorctl;
using ParamId = coriander::base::ParamId;

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
  auto paramReqValidator = std::make_shared<testing::mock::MockParamReqValidator>();

  EncoderElecAngleEstimator estimator(encoder, param, paramReqValidator);

  param->add(Property{4, ParamId::PolePair});
  ASSERT_TRUE(param->has("PolePair"));

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
  auto paramReqValidator = std::make_shared<testing::mock::MockParamReqValidator>();

  EncoderElecAngleEstimator estimator(encoder, param, paramReqValidator);

  param->add(Property{4, ParamId::PolePair});
  param->add(Property{0.0f, ParamId::PersistRawElecAngle});

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
  auto paramReqValidator = std::make_shared<testing::mock::MockParamReqValidator>();

  EncoderElecAngleEstimator estimator(encoder, param, paramReqValidator);

  param->add(Property{4, ParamId::PolePair});
  param->add(Property{0.0f, ParamId::ElecAngleOffset});

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
  using ParamId = coriander::base::ParamId;
  auto encoder = std::make_shared<::dummyEncoder>();
  auto param = std::make_shared<ParameterBase>();
  auto paramReqValidator = std::make_shared<testing::mock::MockParamReqValidator>();

  EncoderElecAngleEstimator estimator(encoder, param, paramReqValidator);

  param->add(Property{4, ParamId::PolePair});
  param->add(Property{10.0f, ParamId::ElecAngleOffset});
  param->add(Property{180.0f, ParamId::PersistRawElecAngle});

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
  using ParamId = coriander::base::ParamId;
  auto encoder = std::make_shared<::dummyEncoder>();
  auto param = std::make_shared<ParameterBase>();
  auto paramReqValidator = std::make_shared<testing::mock::MockParamReqValidator>();

  EncoderElecAngleEstimator estimator(encoder, param, paramReqValidator);

  param->add(Property{4, ParamId::PolePair});
  param->add(Property{0.0f, ParamId::ElecAngleOffset});
  param->add(Property{0.0f, ParamId::PersistRawElecAngle});

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
