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

#include "coriander/base/property.h"
#include "coriander/motorctl/encoder_elec_angle_estimator.h"
#include "tests/mocks.h"

using ParamId = coriander::base::ParamId;
using Parameter = testing::mock::MockPersistentParameter;
using coriander::base::Property;
using coriander::motorctl::EncoderElecAngleEstimator;
using coriander::motorctl::IEncoder;
using testing::mock::MockParamReqValidator;

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
  auto param = std::make_shared<Parameter>();
  auto paramReqValidator = std::make_shared<MockParamReqValidator>();
  auto logger = std::make_shared<testing::mock::MockLogger>();

  EncoderElecAngleEstimator estimator(encoder, param, logger,
                                      paramReqValidator);

  param->add(Property{4, ParamId::MotorCtl_MotorDriver_PolePair});
  param->add(Property{0, ParamId::Sensor_Encoder_ReverseElecAngle});
  ASSERT_TRUE(param->has("MotorCtl_MotorDriver_PolePair"));

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
  auto param = std::make_shared<Parameter>();
  auto paramReqValidator = std::make_shared<MockParamReqValidator>();
  auto logger = std::make_shared<testing::mock::MockLogger>();

  EncoderElecAngleEstimator estimator(encoder, param, logger,
                                      paramReqValidator);

  param->add(Property{4, ParamId::MotorCtl_MotorDriver_PolePair});
  param->add(Property{0.0f, ParamId::MotorCtl_MotorDriver_PersistRawElecAngle});
  param->add(Property{0, ParamId::Sensor_Encoder_ReverseElecAngle});

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
  auto param = std::make_shared<Parameter>();
  auto paramReqValidator = std::make_shared<MockParamReqValidator>();
  auto logger = std::make_shared<testing::mock::MockLogger>();

  EncoderElecAngleEstimator estimator(encoder, param, logger,
                                      paramReqValidator);

  param->add(Property{4, ParamId::MotorCtl_MotorDriver_PolePair});
  param->add(Property{0.0f, ParamId::MotorCtl_Calibrate_CaliElecAngleOffset});
  param->add(Property{0, ParamId::Sensor_Encoder_ReverseElecAngle});

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
  auto param = std::make_shared<Parameter>();
  auto paramReqValidator = std::make_shared<MockParamReqValidator>();
  auto logger = std::make_shared<testing::mock::MockLogger>();

  EncoderElecAngleEstimator estimator(encoder, param, logger,
                                      paramReqValidator);

  param->add(Property{4, ParamId::MotorCtl_MotorDriver_PolePair});
  param->add(Property{10.0f, ParamId::MotorCtl_Calibrate_CaliElecAngleOffset});
  param->add(
      Property{180.0f, ParamId::MotorCtl_MotorDriver_PersistRawElecAngle});
  param->add(Property{0, ParamId::Sensor_Encoder_ReverseElecAngle});

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
  auto param = std::make_shared<Parameter>();
  auto paramReqValidator = std::make_shared<MockParamReqValidator>();
  auto logger = std::make_shared<testing::mock::MockLogger>();

  EncoderElecAngleEstimator estimator(encoder, param, logger,
                                      paramReqValidator);

  param->add(Property{4, ParamId::MotorCtl_MotorDriver_PolePair});
  param->add(Property{0.0f, ParamId::MotorCtl_Calibrate_CaliElecAngleOffset});
  param->add(Property{0.0f, ParamId::MotorCtl_MotorDriver_PersistRawElecAngle});
  param->add(Property{0, ParamId::Sensor_Encoder_ReverseElecAngle});

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
