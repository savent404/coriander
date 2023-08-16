#include <gtest/gtest.h>
#include "coriander/base/property_stream.h"
#include <sstream>

using namespace coriander::base;
using namespace coriander;
TEST(PropertyStream, textStream)
{
    Property p{0, ParamId::MotorCtlMode};
    PropertyTextStream serializer(std::move(p));
    std::stringstream ss;

    ss << serializer;
    EXPECT_STRNE(ss.str().c_str(), "");

    std::cout << ss.str() << std::endl;
}

TEST(PropertyStream, binaryStream)
{
    Property p{0, ParamId::MotorCtlMode};
    Property p1 = p;
    PropertyBinaryStream serializer(std::move(p));
    PropertyBinaryStream serializer1(Property{});
    std::stringstream ss;

    ss << serializer;

    ss >> serializer1;
    EXPECT_EQ(serializer1, serializer);
}