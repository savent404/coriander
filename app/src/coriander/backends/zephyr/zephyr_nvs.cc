/**
 * @file zephyr_nvs.cc
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-02
 *
 * Copyright 2023 savent_gate
 *
 */
#include "zephyr_nvs.h"

#include <zephyr/drivers/flash.h>
#include <zephyr/fs/nvs.h>
#include <zephyr/logging/log.h>
#include <zephyr/storage/flash_map.h>

LOG_MODULE_REGISTER(nvs);

namespace {
#define NVS nvs_partition

static struct nvs_fs fs = {
    .offset = FIXED_PARTITION_OFFSET(NVS),
    .flash_device = FIXED_PARTITION_DEVICE(NVS),
};

static void nvs_init(void) {
  int rc;
  struct flash_pages_info info;
  if (!device_is_ready(fs.flash_device)) {
    LOG_ERR("Flash device %s is not ready", fs.flash_device->name);
    return;
  }

  rc = flash_get_page_info_by_offs(fs.flash_device, fs.offset, &info);
  if (rc) {
    LOG_ERR("Unable to get page info");
    return;
  }
  fs.sector_size = info.size;
  fs.sector_count = FIXED_PARTITION_SIZE(NVS) / info.size;

  rc = nvs_mount(&fs);
  if (rc) {
    LOG_ERR("Unable to mount nvs");
    return;
  }
}

}  // namespace

namespace coriander {
namespace zephyr {

nvs* nvs::getInstance() {
  static nvs instance;
  static bool inited = false;
  if (!inited) {
    nvs_init();
    inited = true;
  }
  return &instance;
}

bool nvs::read(int id, void* data, size_t len) {
  int rc;
  struct nvs_fs* fs = &::fs;
  rc = nvs_read(fs, id, data, len);
  if (rc != int(len)) {
    LOG_ERR("Unable to read nvs id: %d", id);
    return false;
  }
  return true;
}

bool nvs::write(int id, void* data, size_t len) {
  int rc;
  struct nvs_fs* fs = &::fs;
  rc = nvs_write(fs, id, data, len);
  if (rc != int(len)) {
    LOG_ERR("Unable to write nvs id: %d", id);
    return false;
  }
  return true;
}
}  // namespace zephyr
}  // namespace coriander
