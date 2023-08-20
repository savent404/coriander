/**
 * @file itty.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-19
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once

#include <unistd.h>

#include <cstdint>

namespace coriander {
namespace base {

struct ITty {
  virtual ~ITty() = default;
  virtual ssize_t read(uint8_t* buf, size_t size) = 0;
  virtual ssize_t write(uint8_t* buf, size_t size) = 0;
};

}  // namespace base
}  // namespace coriander
