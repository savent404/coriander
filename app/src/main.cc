/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#include <app_version.h>
#include <boost/di.hpp>
#include <memory>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main, CONFIG_APP_LOG_LEVEL);

// interfaces
#include "coriander/application/iappstatus.h"

// implements
#include "coriander/application/appstatus.h"

namespace {

static auto createIocContainer()
{
    return boost::di::make_injector(
        boost::di::bind<coriander::IAppStatus>.to<coriander::AppStatus>().in(boost::di::singleton));
}
}

int main(void)
{
    auto injector = createIocContainer();
    auto appStatus = injector.create<std::shared_ptr<coriander::IAppStatus>>();

    k_sleep(K_MSEC(5000));

    appStatus->setStatus(coriander::IAppStatus::Status::Ok);

    while (1) {
        LOG_INF("Hello World! %s", CONFIG_BOARD);
        k_sleep(K_MSEC(500));
    }
    return 0;
}
