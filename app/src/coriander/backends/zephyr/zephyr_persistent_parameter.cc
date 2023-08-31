/**
 * @file zephyr_persistent_parameter.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-31
 *
 * Copyright 2023 savent_gate
 *
 */
#include "zephyr/zephyr_persistent_parameter.h"

namespace coriander {
namespace zephyr {
// TODO(savent): need to implement
bool PersistentParameter::save() {
  ParameterMemoryMapper mapper;
  bool ret;

  auto map = mapper.map(this);
  if (map.empty()) {
    return false;
  }

  auto nvs_inst = nvs::getInstance();
  if (nvs_inst == nullptr) {
    return false;
  }

  ret = nvs_inst->write(0, map.data(), map.size());
  mapper.unmap();
  return ret;
}

bool PersistentParameter::load() {
  ParameterMemoryMapper mapper;
  constexpr size_t buffer_size = 1024;
  auto buffer = std::make_unique<uint8_t[]>(buffer_size);
  if (buffer == nullptr) {
    return false;
  }

  auto nvs_inst = nvs::getInstance();
  if (nvs_inst == nullptr) {
    return false;
  }

  auto read_size = nvs_inst->read(0, buffer.get(), buffer_size);
  if (read_size <= 0) {
    return false;
  }

  return mapper.recovery(std::span(buffer.get(), read_size), this);
}
}  // namespace zephyr
}  // namespace coriander
