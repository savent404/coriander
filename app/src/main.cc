/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#include <app_version.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "app_version.h"
#include "coriander/coriander.h"

LOG_MODULE_REGISTER(main);

static void loop_hook() { k_sleep(K_MSEC(1)); }

int main(void) {
  LOG_INF("Coriander version %s", APP_VERSION_STRING);
  coriander::coriander_loop(loop_hook);
  return 0;
}
