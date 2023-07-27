/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#include <app_version.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <boost/di.hpp>
#include <memory>
LOG_MODULE_REGISTER(main, CONFIG_APP_LOG_LEVEL);

// interfaces
#include "coriander/application/iappstatus.h"
#include "coriander/iboard_event.h"
#include "coriander/iboard_state.h"

// implements
#include "coriander/application/appstatus.h"
#include "coriander/board_event.h"
#include "coriander/board_state.h"

namespace {

static auto createIocContainer() {
  return boost::di::make_injector(
      boost::di::bind<coriander::IAppStatus>.to<coriander::AppStatus>(),
      boost::di::bind<coriander::IBoardState>.to<coriander::BoardState>(),
      boost::di::bind<coriander::IStateInitHandler>.to<coriander::StateInitHandler>(),
      boost::di::bind<coriander::IStateStandbyHandler>.to<coriander::StateStandbyHandler>(),
      boost::di::bind<coriander::IStateRunHandler>.to<coriander::StateRunHandler>(),
      boost::di::bind<coriander::IStateErrorHandler>.to<coriander::StateErrorHandler>(),
      boost::di::bind<coriander::IStateCalibrateHandler>.to<coriander::StateCalibrateHandler>(),
      boost::di::bind<coriander::IStateFirmwareUpdateHandler>.to<coriander::StateFirmwareUpdateHandler>(),
      boost::di::bind<coriander::IBoardEvent>.to<coriander::BoardEvent>());
}
}  // namespace

int main(void) {
  auto injector = createIocContainer();

  auto board = injector.create<std::shared_ptr<coriander::IBoardState>>();

  while (1) {
    board->loop();
    k_sleep(K_MSEC(1));
  }

  return 0;
}
