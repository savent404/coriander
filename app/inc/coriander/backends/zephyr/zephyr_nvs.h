/**
 * @file zephyr_nvs.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-02
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once

#include <cstddef>

namespace coriander {
namespace zephyr {
struct nvs {
  nvs(const nvs&) = delete;
  nvs& operator=(const nvs&) = delete;

  int read(int id, void* data, size_t len);
  bool write(int id, void* data, size_t len);

  static nvs* getInstance();

 private:
  nvs() = default;
};
}  // namespace zephyr
}  // namespace coriander
