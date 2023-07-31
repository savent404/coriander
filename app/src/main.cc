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
}  // namespace

int main(void) {
  auto injector =
      coriander::coriander_create_injector(zephyr_backends_bindings());

  auto os = coriander::base::LoggerStream(
      injector.create<std::shared_ptr<coriander::base::ILogger>>());
  os << "Coriander version " << APP_VERSION_STRING << std::endl;

  auto diag_regsiter =
      coriander::application::zephyr::DiagnosisRegister::getInstance();
  auto diag =
      injector.create<std::shared_ptr<coriander::application::Diagnosis>>();
  diag_regsiter->applyAll(*diag);

  coriander::coriander_loop(loop_hook, std::move(injector));
  return 0;
}
