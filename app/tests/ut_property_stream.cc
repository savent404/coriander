/**
 * @file ut_property_stream.cc
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief 
 * @date 2023-08-19
 * 
 * Copyright 2023 savent_gate
 * 
 */
#include <gtest/gtest.h>

#include <sstream>

#include "coriander/base/property_stream.h"

using Property = coriander::base::Property;
using PropertyTextStream = coriander::base::PropertyTextStream;
using PropertyBinaryStream = coriander::base::PropertyBinaryStream;
using ParamId = coriander::base::ParamId;

TEST(PropertyStream, textStream) {
  Property p{0, ParamId::MotorCtl_General_Mode_RT};
  PropertyTextStream serializer(std::move(p));
  std::stringstream ss;

  ss << serializer;
  EXPECT_STRNE(ss.str().c_str(), "");

  std::cout << ss.str() << std::endl;
}

TEST(PropertyStream, binaryStream) {
  Property p{0, ParamId::MotorCtl_General_Mode_RT};
  Property p1 = p;
  PropertyBinaryStream serializer(std::move(p));
  PropertyBinaryStream serializer1(Property{});
  std::stringstream ss;

  ss << serializer;

  ss >> serializer1;
  EXPECT_EQ(serializer1, serializer);
}
