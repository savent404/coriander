/**
 * @file ut_parameters.cc
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief 
 * @date 2023-08-01
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "coriander/parameters.h"
#include <gtest/gtest.h>

TEST(Parameter, basic)
{
    using namespace coriander;
    using namespace coriander::base;

    ParameterBase param;
    ParameterMemoryMapper mapper;

    param.add(Property{0, "t1", "null for t1"});
    param.add(Property{1, "t2", "null for t2"});

    auto map = mapper.map(&param);
    ASSERT_TRUE(mapper.isValid(map));

    auto mirror = std::make_unique<uint8_t[]>(map.size());
    std::memcpy(mirror.get(), map.data(), map.size());

    mapper.unmap();

    ParameterBase mirror_param;
    ASSERT_TRUE(mapper.recovery(std::span<uint8_t>{mirror.get(), map.size()}, &mirror_param));

    ASSERT_TRUE(mirror_param.has("t1"));
    ASSERT_TRUE(mirror_param.has("t2"));
    ASSERT_FALSE(mirror_param.has("t3"));
    ASSERT_EQ(get<int>(mirror_param.get("t1").value()), 0);
    ASSERT_EQ(get<int>(mirror_param.get("t2").value()), 1);
}