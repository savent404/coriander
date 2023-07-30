/**
 * @file container.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-07-30
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once

#include <memory.h>

#include <boost/di.hpp>

// dependencies
#include "coriander/application/appstatus.h"
#include "coriander/board_event.h"
#include "coriander/board_state.h"
#include "coriander/board_state_calibrate_handler.h"
#include "coriander/board_state_error_handler.h"
#include "coriander/board_state_firmware_update_handler.h"
#include "coriander/board_state_init_handler.h"
#include "coriander/board_state_run_handler.h"
#include "coriander/board_state_standby_handler.h"

namespace coriander {

namespace detail {

template <typename T_injector, typename... T_args>
static inline auto extendContainer(T_injector&& injector, T_args&&... args) {
  return boost::di::make_injector(std::forward<T_injector>(injector),
                                  std::forward<T_args>(args)...);
}

static inline auto defaultContainer() {
  return boost::di::make_injector(
      boost::di::bind<coriander::IAppStatus>.to<coriander::AppStatus>(),
      boost::di::bind<coriander::IBoardState>.to<coriander::BoardState>(),
      boost::di::bind<coriander::IBoardStateInitHandler>.to<coriander::BoardStateInitHandler>(),
      boost::di::bind<coriander::IBoardStateStandbyHandler>.to<coriander::BoardStateStandbyHandler>(),
      boost::di::bind<coriander::IBoardStateRunHandler>.to<coriander::BoardStateRunHandler>(),
      boost::di::bind<coriander::IBoardStateErrorHandler>.to<coriander::BoardStateErrorHandler>(),
      boost::di::bind<coriander::IBoardStateCalibrateHandler>.to<coriander::BoardStateCalibrateHandler>(),
      boost::di::bind<coriander::IBoardStateFirmwareUpdateHandler>.to<coriander::BoardStateFirmwareUpdateHandler>(),
      boost::di::bind<coriander::IBoardEvent>.to<coriander::BoardEvent>());
}

template <typename... T_args>
static inline auto createInjector(T_args&&... args) {
  return boost::di::make_injector(std::move(defaultContainer()),
                                  std::forward<T_args>(args)...);
}

}  // namespace detail
}  // namespace coriander
