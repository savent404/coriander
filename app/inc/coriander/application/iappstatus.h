/**
 * @file iappstatus.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-07-27
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once

namespace coriander {
namespace application {
struct IAppStatus {
  enum class Status {
    Ok,
    Busy,
    Error,
  };

  virtual ~IAppStatus() = default;

  virtual void setStatus(Status status) noexcept = 0;
  virtual Status getStatus() const noexcept = 0;
};

}  // namespace application
}  // namespace coriander
