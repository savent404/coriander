/**
 * @file zephyr_shell_protocol.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-12
 *
 * Copyright 2023 savent_gate
 *
 */
#include "zephyr_shell_protocol.h"

#include <zephyr/shell/shell.h>

namespace {
static std::shared_ptr<coriander::BoardCtl> ctl = nullptr;
}

namespace coriander {
namespace zephyr {
void ShellProtocol::enable() { ctl = mBoardCtl; }
void ShellProtocol::disable() { ctl = nullptr; }
void ShellProtocol::loop() {}
}  // namespace zephyr
}  // namespace coriander

extern "C" {
static int shell_enable_motor(const struct shell* shell, size_t argc,
                              char** argv) {
  if (!ctl) {
    return -1;
  }
  ctl->motorEnable();
  return 0;
}

static int shell_disable_motor(const struct shell* shell, size_t argc,
                               char** argv) {
  if (!ctl) {
    return -1;
  }
  ctl->motorDisable();
  return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(
    motor_cmd,
    SHELL_CMD_ARG(enable, NULL, "enable motor", shell_enable_motor, 1, 0),
    SHELL_CMD_ARG(disable, NULL, "disable motor", shell_disable_motor, 1, 0),
    SHELL_SUBCMD_SET_END /* Array terminated. */
);

SHELL_CMD_REGISTER(motor, &motor_cmd, "motor control", NULL);
}