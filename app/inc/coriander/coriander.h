/**
 * @file coriander.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-07-30
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once

#include <memory>
#include <utility>

#include "coriander/container.h"

namespace coriander {

using CorianderMainObject = std::shared_ptr<coriander::IBoardState>;

/**
 * @brief Create injector with default container
 *
 * @tparam T_args type of user defined di bindings
 * @param args user defined di bindings
 * @return auto injector
 */
template <typename... T_args>
static inline auto coriander_create_injector(T_args&&... args) {
  return boost::di::make_injector(
      std::move(coriander::detail::defaultContainer()),
      std::forward<T_args>(args)...);
}

/**
 * @brief main loop of coriander
 *
 * @tparam T_hook  type of hook function in main loop
 * @tparam T_injector type of injector
 * @param hook hook function in main loop
 * @param injector injector of coriander
 * @return auto none
 */
template <typename T_hook, typename T_injector>
static inline auto coriander_loop(T_hook hook, T_injector&& injector) {
  auto obj = injector.template create<CorianderMainObject>();
  for (;;) {
    obj->loop();
    hook();
  }
}
}  // namespace coriander
