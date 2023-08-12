/**
 * @file zephyr_shell_protocol.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-12
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once

#include "coriander/board_ctl.h"
#include "coriander/iprotocol_ctl.h"

namespace coriander {
namespace zephyr {
struct ShellProtocol : public IProtocolCtl {
  using BoardCtl = coriander::BoardCtl;

  ShellProtocol(std::shared_ptr<BoardCtl> boardCtl) : mBoardCtl(boardCtl) {}
  virtual void enable() noexcept;
  virtual void disable() noexcept;
  virtual void loop() noexcept;

 private:
  std::shared_ptr<BoardCtl> mBoardCtl;
};
}  // namespace zephyr
}  // namespace coriander