/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#include <app_version.h>
#include <memory>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main, CONFIG_APP_LOG_LEVEL);

#include "coriander/application/appstatus.h"

int main(void)
{
    auto appStatus = std::make_unique<coriander::AppStatus>();
    k_sleep(K_MSEC(5000));
    appStatus->setStatus(coriander::AppStatus::Status::Ok);
    while (1) {
        LOG_INF("Hello World! %s", CONFIG_BOARD);
        k_sleep(K_MSEC(500));
    }
    return 0;
}
