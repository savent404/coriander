/**
 * @file proto_td2kw_rx_frame.cc
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-20
 *
 * Copyright 2023 savent_gate
 *
 */
#include "coriander/protos/proto_td2kw.h"

namespace coriander {
namespace proto {
namespace td2kw_detail {

RxFrameParser::RxFrameParser() : mBuf(16) {}

bool RxFrameParser::parse(std::span<uint8_t> buf) {
  for (auto byte : buf) {
    if (parseOneByte(byte)) {
      return true;
    }
  }
  return false;
}

bool RxFrameParser::parseOneByte(uint8_t byte) {
  ssize_t cur_idx = mBuf.size();
  bool accept = false;

  switch (cur_idx) {
    case 0:
      if (byte == 0xEB) {
        accept = true;
      }
      break;
    case 1:
      if (byte == 0x90) {
        accept = true;
      }
      break;
    case 2 ... 11:
      accept = true;
      break;
    case 12:
      if (byte == getChecksum()) {
        accept = true;
      }
      break;
    default:
      break;
  }

  if (!accept) {
    /** discard all bytes until header1(0xEB)
     * NOTE(savent): there is possible that the header2 is invalid
     *               and it just waste time to discard again, but
     *               it's not a big deal cause no data loss.
     */
    while (mBuf.size() && mBuf[0] != 0xEB) {
      mBuf.pop_front();
    }
  } else {
    mBuf.push_back(byte);
  }

  // byte accepted and frame is complete
  return accept && mBuf.size() == 13;
}

uint8_t RxFrameParser::getChecksum() const {
  uint8_t checksum = 0;
  if (mBuf.size() != 12) {
    return checksum;
  }
  // checksum = sum of [2...11](exclude checksum and sync bytes)
  for (int idx = 2; idx < 11; ++idx) {
    checksum += mBuf.at(idx);
  }
  return checksum;
}

size_t RxFrameParser::requiredBytesSize() const { return 13 - mBuf.size(); }

uint8_t RxFrameParser::getCounter() const { return mBuf.at(2); }

bool RxFrameParser::getEnable() const {
  uint8_t b = mBuf.at(3);
  bool res;
  switch (b) {
    case 0xF0:
      res = true;
      break;
    case 0x0F:
    default:
      res = false;
      break;
  }
  return res;
}

float RxFrameParser::getTargetPosition() const {
  uint8_t bl = mBuf.at(4);
  uint8_t bh = mBuf.at(5);
  uint16_t b = (bh << 8) | bl;
  auto f = static_cast<float>(b) / 10.0f;  // lsb 0.1 degree
  return f;
}

}  // namespace td2kw_detail
}  // namespace proto
}  // namespace coriander
