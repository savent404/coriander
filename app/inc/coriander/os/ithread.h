/**
 * @file ithread.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-10
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once

#include <cstddef>

namespace coriander {
namespace os {

struct IThread {
  virtual ~IThread() = default;
  virtual size_t currentThreadId() = 0;
};

}  // namespace os
}  // namespace coriander
