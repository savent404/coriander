/**
 * @file ut_proto_td2kw.cc
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-20
 *
 * Copyright 2023 savent_gate
 *
 */
#include <gtest/gtest.h>

#include "coriander/protos/proto_td2kw.h"

TEST(ProtoTd2kw, basic) {
  boost::circular_buffer<uint8_t> t(16);

  t.push_back(1);
}
