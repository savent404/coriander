/**
 * @file zephyr_persistent_parameter.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-31
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once

#include <memory>

#include "coriander/parameters.h"
#include "zephyr/zephyr_nvs.h"

namespace coriander {
namespace zephyr {
struct PersistentParameter : public IPersistentParameter {
  virtual bool save();
  virtual bool load();
};

}  // namespace zephyr
}  // namespace coriander
