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

#include <utility>

#include "coriander/container.h"

namespace coriander {

using CorianderMainObject = std::shared_ptr<coriander::IBoardState>;

template <typename T_hook, typename... T_args>
static inline auto coriander_loop(T_hook hook, T_args&&... args) {
  auto injector = boost::di::make_injector(
      std::move(detail::defaultContainer()), std::forward<T_args>(args)...);
  auto obj = injector.template create<CorianderMainObject>();
  for (;;) {
    obj->loop();
    hook();
  }
}
}  // namespace coriander
