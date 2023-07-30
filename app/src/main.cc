/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#include <app_version.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include "coriander/coriander.h"

int main(void) {
  auto instance = create_coriander_instance();
  while (1) {
    coriander_run(instance);
    k_sleep(K_MSEC(1));
  }

  return 0;
}
