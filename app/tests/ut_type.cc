#include "coriander/base/type.h"
#include <gtest/gtest.h>

using namespace coriander::base;

TEST(Type, foo)
{
    EXPECT_EQ(1, 1);
}

TEST(Type, basic)
{
    using namespace coriander;

    EXPECT_EQ(TypeId::Int32, TypeHelper::type("int32_t"));
    EXPECT_EQ(TypeId::Int64, TypeHelper::type("int64_t"));
    EXPECT_EQ(TypeId::UInt32, TypeHelper::type("uint32_t"));
    EXPECT_EQ(TypeId::UInt64, TypeHelper::type("uint64_t"));
    EXPECT_EQ(TypeId::Float, TypeHelper::type("float"));
    EXPECT_EQ(TypeId::Double, TypeHelper::type("double"));
    EXPECT_EQ(TypeId::String, TypeHelper::type("string"));
    EXPECT_EQ(TypeId::TripleInt32, TypeHelper::type("Triple<int32_t>"));
    EXPECT_EQ(TypeId::TripleFloat, TypeHelper::type("Triple<float>"));
    EXPECT_EQ(TypeId::TripleDouble, TypeHelper::type("Triple<double>"));
    EXPECT_EQ(TypeId::Invalid, TypeHelper::type("Invalid"));
}

TEST(Type, convert)
{
    using namespace coriander;

    Type t1 { 1 };
    EXPECT_EQ(size_t(TypeId::Int32), t1.index());
    EXPECT_EQ(TypeHelper::type(t1), TypeId::Int32);

    Type t2 { 1L };
    EXPECT_EQ(size_t(TypeId::Int64), t2.index());
    EXPECT_EQ(TypeHelper::type(t2), TypeId::Int64);

    Type t3 { 1U };
    EXPECT_EQ(size_t(TypeId::UInt32), t3.index());
    EXPECT_EQ(TypeHelper::type(t3), TypeId::UInt32);

    Type t4 { 1UL };
    EXPECT_EQ(size_t(TypeId::UInt64), t4.index());
    EXPECT_EQ(TypeHelper::type(t4), TypeId::UInt64);

    Type t5 { 1.0f };
    EXPECT_EQ(size_t(TypeId::Float), t5.index());
    EXPECT_EQ(TypeHelper::type(t5), TypeId::Float);

    Type t6 { 1.0 };
    EXPECT_EQ(size_t(TypeId::Double), t6.index());
    EXPECT_EQ(TypeHelper::type(t6), TypeId::Double);

    Type t7 { "1" };
    EXPECT_EQ(size_t(TypeId::String), t7.index());
    EXPECT_EQ(TypeHelper::type(t7), TypeId::String);

    Type t8 = make_tuple(1, 2, 3);
    EXPECT_EQ(size_t(TypeId::TripleInt32), t8.index());
    EXPECT_EQ(TypeHelper::type(t8), TypeId::TripleInt32);

    Type t9 = make_tuple(1.0f, 2.0f, 3.0f);
    EXPECT_EQ(size_t(TypeId::TripleFloat), t9.index());
    EXPECT_EQ(TypeHelper::type(t9), TypeId::TripleFloat);

    Type t10 = make_tuple(1.0, 2.0, 3.0);
    EXPECT_EQ(size_t(TypeId::TripleDouble), t10.index());
    EXPECT_EQ(TypeHelper::type(t10), TypeId::TripleDouble);
}
