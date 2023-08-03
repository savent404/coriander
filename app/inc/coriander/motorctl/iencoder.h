/**
 * @file iencoder.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-03
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

#include "coriander/motorctl/isensor.h"

namespace coriander {
namespace motorctl {
struct IEncoder : public ISensor {
  virtual unsigned getEncoderCount() = 0;
  virtual unsigned getEncoderCountPerRound() = 0;
  virtual int getOverflowCount() = 0;
};
}  // namespace motorctl
}  // namespace coriander
