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
#include "posix_logger.h"

TEST(Parameter, basic) {
  using namespace coriander;
  using namespace coriander::base;
  using ParamId = coriander::base::ParamId;

  ParameterBase param;

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
  using namespace coriander;
  using namespace coriander::base;
  using ParamId = coriander::base::ParamId;

  ParameterBase param;
  ParameterMemoryMapper mapper;

  param.add(Property{0, ParamId::CalibrateDuration});
  param.add(Property{1, ParamId::CalibrateVoltage});

  auto map = mapper.map(&param);
  ASSERT_TRUE(mapper.isValid(map));

  auto mirror = std::make_unique<uint8_t[]>(map.size());
  std::memcpy(mirror.get(), map.data(), map.size());

  mapper.unmap();

  ParameterBase mirror_param;
  ASSERT_TRUE(mapper.recovery(std::span<uint8_t>{mirror.get(), map.size()},
                              &mirror_param));

  ASSERT_TRUE(mirror_param.has("CalibrateDuration"));
  ASSERT_TRUE(mirror_param.has("CalibrateVoltage"));
  ASSERT_FALSE(mirror_param.has("t3"));
  ASSERT_EQ(get<int>(mirror_param.get("CalibrateDuration").value()), 0);
  ASSERT_EQ(get<int>(mirror_param.get("CalibrateVoltage").value()), 1);
  ASSERT_EQ(mirror_param.getValue<int>("CalibrateDuration"), 0);
  ASSERT_EQ(mirror_param.getValue<int>("CalibrateVoltage"), 1);
}

namespace {
struct Foo : public coriander::IParamReq {
 protected:
  virtual const coriander::ParameterRequireItem* requiredParameters() const {
    constexpr static const coriander::ParameterRequireItem items[] = {
        {"Unknow", coriander::TypeId::Int32},
        {"CalibrateDuration", coriander::TypeId::Float},
        {"CalibrateVoltage", coriander::TypeId::String},
        {nullptr, coriander::TypeId::Invalid}};

    return &items[0];
  }
};
}  // namespace

TEST(Parameter, requirements) {
  using Property = coriander::Property;
  using ParamId = coriander::base::ParamId;
  auto param = std::make_shared<coriander::ParameterBase>();
  auto validator = std::make_shared<coriander::ParamReqValidator>(
      std::make_shared<coriander::base::posix::Logger>(), param);
  auto foo = std::make_shared<Foo>();

  validator->addParamReq(foo.get());
  ASSERT_FALSE(validator->validate());

  param->add(Property{0, ParamId::Unknow});
  param->add(Property{1.0f, ParamId::CalibrateDuration});
  param->add(Property{"foo", ParamId::CalibrateVoltage});
  ASSERT_EQ(validator->validate(), true);
}
