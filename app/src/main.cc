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
#include "coriander/coriander.h"
#include "zephyr_logger.h"

LOG_MODULE_REGISTER(main);

namespace {

static void loop_hook() { k_sleep(K_MSEC(1)); }

using namespace boost;
static auto zephyr_backends_bindings() {
  return di::make_injector(
      di::bind<coriander::base::ILogger>.to<coriander::base::zephyr::Logger>());
}
}  // namespace

#include "coriander/base/loggerstream.h"
int main(void) {
  LOG_INF("Coriander version %s", APP_VERSION_STRING);
  coriander::coriander_loop(loop_hook, zephyr_backends_bindings());
  return 0;
}
