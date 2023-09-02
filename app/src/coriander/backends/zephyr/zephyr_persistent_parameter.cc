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

#include <zephyr/fs/fs.h>
#include <zephyr/logging/log.h>

#ifdef CONFIG_CORIANDER_MINIMAL
namespace coriander::zephyr {
bool PersistentParameter::save() { return true; }
bool PersistentParameter::load() { return true; }
}  // namespace coriander::zephyr
#else

LOG_MODULE_REGISTER(persistent, CONFIG_APP_LOG_LEVEL);

namespace coriander {
namespace zephyr {
// TODO(savent): need to implement
bool PersistentParameter::save() {
  ParameterMemoryMapper mapper;
  struct fs_file_t f = {0};
  int ret;

  auto map = mapper.map(this);
  if (map.empty()) {
    return false;
  }

  ret = fs_open(&f, "/fs/param.bin", FS_O_CREATE | FS_O_WRITE);
  if (ret) {
    LOG_ERR("Failed to open file, error: %d", ret);
    return false;
  }

  ret = fs_write(&f, map.data(), map.size());
  if (ret < 0) {
    LOG_ERR("Failed to write file, error: %d", ret);
    return false;
  }

  ret = fs_close(&f);
  if (ret < 0) {
    LOG_ERR("Failed to close file, error: %d", ret);
    return false;
  }

  mapper.unmap();
  return true;
}

bool PersistentParameter::load() {
  ParameterMemoryMapper mapper;
  constexpr size_t buffer_size = 1024;
  int read_size;
  struct fs_file_t f = {0};
  int ret;
  auto buffer = std::make_unique<uint8_t[]>(buffer_size);
  if (buffer == nullptr) {
    return false;
  }

  ret = fs_open(&f, "/fs/param.bin", FS_O_READ);
  if (ret) {
    LOG_ERR("Failed to open file, error: %d", ret);
    return false;
  }

  ret = fs_read(&f, buffer.get(), buffer_size);
  if (ret < 0) {
    LOG_ERR("Failed to read file, error: %d", ret);
    return false;
  }
  read_size = ret;

  ret = fs_close(&f);

  if (ret < 0) {
    LOG_ERR("Failed to close file, error: %d", ret);
    return false;
  }

  return mapper.recovery(std::span(buffer.get(), read_size), this);
}
}  // namespace zephyr
}  // namespace coriander

#endif
