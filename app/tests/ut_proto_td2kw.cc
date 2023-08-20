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
#include "tests/mocks.h"

using coriander::proto::td2kw_detail::RxFrameParser;

TEST(ProtoTd2kw, rawRxFrame_Parse) {
  RxFrameParser parser;

  uint8_t invalid_data[13] = {0};
  uint8_t valid_data[13] = {0xEB, 0x90, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  // input two frame, and the first frame is invalid
  ASSERT_FALSE(parser.parse(std::span<uint8_t>(invalid_data, 13)));
  ASSERT_TRUE(parser.parse(std::span<uint8_t>(valid_data, 13)));

  // input two frame, and the first frame invalid
  valid_data[12] = 0xEB;
  ASSERT_FALSE(parser.parse(std::span<uint8_t>(valid_data, 13)));
  valid_data[12] = 0x00;
  ASSERT_TRUE(parser.parse(std::span<uint8_t>(valid_data, 13)));

  // input two frame, and the first frame lost a byte
  ASSERT_FALSE(parser.parse(std::span<uint8_t>(valid_data, 12)));
  ASSERT_TRUE(parser.parse(std::span<uint8_t>(valid_data, 13)));

  // input one frame, but with a invalid byte of the head.
  ASSERT_FALSE(parser.parse(std::span<uint8_t>(valid_data, 1)));
  ASSERT_TRUE(parser.parse(std::span<uint8_t>(valid_data, 13)));
}

TEST(ProtoTd2kw, rawRxFrame_Data) {
  RxFrameParser parser;
  uint8_t valid_data[13] = {0xEB, 0x90, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  for (uint8_t i = 0; i < 255; i++) {
    valid_data[2] = i;
    valid_data[12] = i;
    ASSERT_TRUE(parser.parse(std::span<uint8_t>(valid_data, 13)));
    ASSERT_EQ(parser.getCounter(), i);
  }

  valid_data[2] = 0;
  valid_data[12] = 0;
  ASSERT_TRUE(parser.parse(std::span<uint8_t>(valid_data, 13)));
  ASSERT_FALSE(parser.getEnable());

  valid_data[3] = 0x0F;
  valid_data[12] = 0x0F;
  ASSERT_TRUE(parser.parse(std::span<uint8_t>(valid_data, 13)));
  ASSERT_FALSE(parser.getEnable());

  valid_data[3] = 0xF0;
  valid_data[12] = 0xF0;
  ASSERT_TRUE(parser.parse(std::span<uint8_t>(valid_data, 13)));
  ASSERT_TRUE(parser.getEnable());

  valid_data[3] = 0xFF;
  valid_data[12] = 0xFF;
  ASSERT_TRUE(parser.parse(std::span<uint8_t>(valid_data, 13)));
  ASSERT_FALSE(parser.getEnable());

  valid_data[3] = 0;

  for (int i = 0; i < 65535; i++) {
    uint16_t t = i;
    uint8_t bl = t & 0xFF;
    uint8_t bh = (t >> 8) & 0xFF;

    valid_data[4] = bl;
    valid_data[5] = bh;
    valid_data[12] = bl + bh;

    ASSERT_TRUE(parser.parse(std::span<uint8_t>(valid_data, 13)));
    ASSERT_EQ(parser.getTargetPosition(), static_cast<float>(i) / 10.0f);
  }
}
