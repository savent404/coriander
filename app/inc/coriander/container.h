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

#include <utility>

#include "boost/di.hpp"
// dependencies
#include "coriander/board_event.h"
#include "coriander/board_state.h"
#include "coriander/board_state_calibrate_handler.h"
#include "coriander/board_state_error_handler.h"
#include "coriander/board_state_firmware_update_handler.h"
#include "coriander/board_state_init_handler.h"
#include "coriander/board_state_reboot_handler.h"
#include "coriander/board_state_run_handler.h"
#include "coriander/board_state_standby_handler.h"
#include "coriander/iboard_state_calibrate_handler.h"
#include "coriander/iboard_state_firmware_update_handler.h"
#include "coriander/motorctl/container.h"
#include "coriander/parameter_requirements.h"
#include "coriander/parameter_requirements_validator.h"

namespace coriander {

namespace detail {

/**
 * @brief Create injector with default container
 *
 * @tparam T_injector previous injector
 * @tparam T_args type of user defined di bindings
 * @param injector previous injector
 * @param args user defined di bindings
 * @return auto injector
 */
template <typename T_injector, typename... T_args>
static inline auto extendContainer(T_injector&& injector, T_args&&... args) {
  return boost::di::make_injector(std::forward<T_injector>(injector),
                                  std::forward<T_args>(args)...);
}

/**
 * @brief Create coriander default injector
 *
 * @return auto injector
 */
static inline auto createInjector() {
  using boost::di::bind;
  using coriander::IBoardEvent;
  using coriander::IBoardState;
  using coriander::IBoardStateCalibrateHandler;
  using coriander::IBoardStateErrorHandler;
  using coriander::IBoardStateFirmwareUpdateHandler;
  using coriander::IBoardStateInitHandler;
  using coriander::IBoardStateRebootHandler;
  using coriander::IBoardStateRunHandler;
  using coriander::IBoardStateStandbyHandler;
  using coriander::IParamReqValidator;
  using bsfuh = coriander::BoardStateFirmwareUpdateHandler;
  using bsch = coriander::BoardStateCalibrateHandler;

  return boost::di::make_injector(
      bind<IBoardState>.to<coriander::BoardState>(),
      bind<IBoardStateInitHandler>.to<coriander::BoardStateInitHandler>(),
      bind<IBoardStateStandbyHandler>.to<coriander::BoardStateStandbyHandler>(),
      bind<IBoardStateRunHandler>.to<coriander::BoardStateRunHandler>(),
      bind<IBoardStateErrorHandler>.to<coriander::BoardStateErrorHandler>(),
      bind<IBoardStateCalibrateHandler>.to<bsch>(),
      bind<IBoardStateFirmwareUpdateHandler>.to<bsfuh>(),
      bind<IBoardStateRebootHandler>.to<coriander::BoardStateRebootHandler>(),
      bind<IBoardEvent>.to<coriander::BoardEvent>(),
      bind<IParamReqValidator>.to<coriander::ParamReqValidator>());
}

/**
 * @brief Create coriander default injector
 *
 * @return auto injector
 */
static inline auto defaultContainer() {
  return boost::di::make_injector(
      std::move(coriander::detail::createInjector()),
      std::move(coriander::motorctl::detail::createInjector()));
}

}  // namespace detail
}  // namespace coriander
