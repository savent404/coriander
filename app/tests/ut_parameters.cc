/**
 * @file ut_parameters.cc
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-01
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <gtest/gtest.h>

#include "coriander/parameter_requirements.h"
#include "coriander/parameter_requirements_validator.h"
#include "coriander/parameters.h"
#include "posix/posix_logger.h"

using ParamId = coriander::base::ParamId;
using Parameter = coriander::Parameter;
using ParameterMemoryMapper = coriander::ParameterMemoryMapper;
using Property = coriander::base::Property;
TEST(Parameter, basic) {
  Parameter param;

  ASSERT_EQ(param.has("Unknow"), false);
  param.add(Property{0, ParamId::Unknow});
  ASSERT_EQ(param.has("Unknow"), true);
  ASSERT_EQ(param.has(ParamId::Unknow), true);
  ASSERT_EQ(param.getValue<int>("Unknow"), 0);
  ASSERT_EQ(param.getValue<int>(ParamId::Unknow), 0);
  ASSERT_EQ(param.setValue("Unknow", 1), true);
  ASSERT_EQ(param.getValue<int>("Unknow"), 1);
  ASSERT_EQ(param.setValue("t2", 1), false);

  param.remove(ParamId::Unknow);
  ASSERT_EQ(param.has("Unknow"), false);
}

TEST(Parameter, basic_map) {
  Parameter param;
  ParameterMemoryMapper mapper;

  for (int i = 0; i < ParamId::MAX_PARAM_ID; i++) {
    param.add(Property{i, ParamId::_from_index_nothrow(i).value()});
  }

  auto map = mapper.map(&param);
  ASSERT_TRUE(mapper.isValid(map));

  auto mirror = std::make_unique<uint8_t[]>(map.size());
  std::memcpy(mirror.get(), map.data(), map.size());

  mapper.unmap();

  Parameter mirror_param;
  ASSERT_TRUE(mapper.recovery(std::span<uint8_t>{mirror.get(), map.size()},
                              &mirror_param));

  ASSERT_TRUE(mirror_param.has("MotorCtl_Calibrate_CaliDuration"));
  ASSERT_TRUE(mirror_param.has("MotorCtl_Calibrate_CaliVoltage"));
  ASSERT_FALSE(mirror_param.has("t3"));
  ASSERT_EQ(
      get<int>(mirror_param.get("MotorCtl_Calibrate_CaliDuration").value()),
      ParamId::MotorCtl_Calibrate_CaliDuration);
  ASSERT_EQ(
      get<int>(mirror_param.get("MotorCtl_Calibrate_CaliVoltage").value()),
      ParamId::MotorCtl_Calibrate_CaliVoltage);
  ASSERT_EQ(mirror_param.getValue<int>("MotorCtl_Calibrate_CaliDuration"),
            ParamId::MotorCtl_Calibrate_CaliDuration);
  ASSERT_EQ(mirror_param.getValue<int>("MotorCtl_Calibrate_CaliVoltage"),
            ParamId::MotorCtl_Calibrate_CaliVoltage);

  for (int i = 0; i < ParamId::MAX_PARAM_ID; i++) {
    auto id = ParamId::_from_index_nothrow(i).value();
    ASSERT_EQ(mirror_param.getValue<int>(id), i);
  }
}

namespace {
struct Foo : public coriander::IParamReq {
 protected:
  virtual const coriander::ParameterRequireItem* requiredParameters() const {
    constexpr static const coriander::ParameterRequireItem items[] = {
        {"Unknow", coriander::TypeId::Int32},
        {"MotorCtl_Calibrate_CaliDuration", coriander::TypeId::Float},
        {"MotorCtl_Calibrate_CaliVoltage", coriander::TypeId::String},
        {nullptr, coriander::TypeId::Invalid}};

    return &items[0];
  }
};
}  // namespace

TEST(Parameter, requirements) {
  auto param = std::make_shared<coriander::Parameter>();
  auto validator = std::make_shared<coriander::ParamReqValidator>(
      std::make_shared<coriander::base::posix::Logger>(), param);
  auto foo = std::make_shared<Foo>();

  validator->addParamReq(foo.get());
  ASSERT_FALSE(validator->validate());

  param->add(Property{0, ParamId::Unknow});
  param->add(Property{1.0f, ParamId::MotorCtl_Calibrate_CaliDuration});
  param->add(Property{"foo", ParamId::MotorCtl_Calibrate_CaliVoltage});
  ASSERT_EQ(validator->validate(), true);
}
