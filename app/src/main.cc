/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#include <app_version.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <boost/di.hpp>

#include "app_version.h"
#include "coriander/application/diagnosis.h"
#include "coriander/base/loggerstream.h"
#include "coriander/coriander.h"

// backends
#include <zephyr/drivers/flash.h>
#include <zephyr/fs/nvs.h>
#include <zephyr/storage/flash_map.h>

#include "zephyr_appstatus.h"
#include "zephyr_diagnosis.h"
#include "zephyr_logger.h"

LOG_MODULE_REGISTER(main);

namespace {

static void loop_hook() { k_sleep(K_MSEC(1)); }

using namespace boost;
static auto zephyr_backends_bindings() {
  return di::make_injector(
      di::bind<coriander::base::ILogger>.to<coriander::base::zephyr::Logger>(),
      di::bind<coriander::application::IAppStatus>.to<coriander::application::zephyr::AppStatus>());
}

static int get_reboot_times() {
  struct nvs_fs fs;
  struct flash_pages_info info;
  int rc, count;

#define NVS_PARTITION nvs_partition
  fs.flash_device = FIXED_PARTITION_DEVICE(NVS_PARTITION);
  fs.offset = FIXED_PARTITION_OFFSET(NVS_PARTITION);

  if (!device_is_ready(fs.flash_device)) {
    LOG_ERR("Flash device %s is not ready", fs.flash_device->name);
    return -1;
  }

  rc = flash_get_page_info_by_offs(fs.flash_device, fs.offset, &info);
  if (rc) {
    LOG_ERR("Unable to get page info");
    return -1;
  }
  fs.sector_size = info.size;
  fs.sector_count = FIXED_PARTITION_SIZE(NVS_PARTITION) / info.size;

  rc = nvs_mount(&fs);
  if (rc) {
    LOG_ERR("Unable to mount nvs");
    return -1;
  }

  rc = nvs_read(&fs, 0, &count, sizeof(rc));

  if (rc > 0) {
    count = count + 1;
  } else {
    LOG_WRN("Unable to read reboot count, assuming 0");
    count = 0;
  }

  rc = nvs_write(&fs, 0, &count, sizeof(rc));
  if (rc != sizeof(count)) {
    LOG_ERR("Unable to write reboot count");
    return -1;
  }

  LOG_INF("Free space on nvs: 0x%x", nvs_calc_free_space(&fs));

  return count;
}

}  // namespace

int main(void) {
  auto injector =
      coriander::coriander_create_injector(zephyr_backends_bindings());

  auto os = coriander::base::LoggerStream(
      injector.create<std::shared_ptr<coriander::base::ILogger>>());
  os << "Coriander version " << APP_VERSION_STRING << std::endl;
  os << "Reboot times: " << get_reboot_times() << std::endl;

  auto diag_regsiter =
      coriander::application::zephyr::DiagnosisRegister::getInstance();
  auto diag =
      injector.create<std::shared_ptr<coriander::application::Diagnosis>>();
  diag_regsiter->applyAll(*diag);

  coriander::coriander_loop(loop_hook, std::move(injector));
  return 0;
}
