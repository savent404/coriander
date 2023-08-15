#include <gtest/gtest.h>

#include "coriander/base/param.h"
TEST(foo, boo) {
  ASSERT_EQ(1, 1);
  const char *str = (+coriander::base::ParamId::Unknow)._to_string();
  coriander::base::ParamId id = coriander::base::ParamId::_from_string(str);
  ASSERT_EQ(id, (+coriander::base::ParamId::Unknow));
}