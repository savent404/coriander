/**
 * @file appstatus.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-07-27
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once

#include "coriander/application/iappstatus.h"

namespace coriander {

namespace application {
namespace zephyr {
struct AppStatus : public IAppStatus {
  AppStatus(const AppStatus&) = delete;
  AppStatus(AppStatus&&) = delete;
  virtual AppStatus& operator=(const AppStatus&) = delete;

  AppStatus() noexcept;
  void setStatus(Status status) noexcept override;
  Status getStatus() const noexcept override;
};
}  // namespace zephyr
}  // namespace application
}  // namespace coriander
