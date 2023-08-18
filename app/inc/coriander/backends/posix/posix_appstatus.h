/**
 * @file posix_appstatus.h
 * @author Z004TPFF (yuankai.liao@siemens.com)
 * @brief
 * @date 2023-07-31
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once

#include "coriander/application/iappstatus.h"

namespace coriander {
namespace application {

namespace posix {
struct AppStatus : public IAppStatus {
  Status mStatus = Status::Ok;

  void setStatus(Status status) noexcept override { mStatus = status; }

  Status getStatus() const noexcept override { return mStatus; }
};
}  // namespace posix
}  // namespace application
}  // namespace coriander
