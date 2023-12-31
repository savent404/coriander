/**
 * @file zephyr_shell_protocol.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-12
 *
 * Copyright 2023 savent_gate
 *
 */
#include "zephyr/zephyr_shell_protocol.h"

#include <zephyr/fs/fs.h>
#include <zephyr/shell/shell.h>

#include <cstring>
#include <functional>

#include "coriander/base/property_set.h"
#include "coriander/base/property_stream.h"
#include "coriander/base/type.h"

namespace {
static std::shared_ptr<coriander::BoardCtl> ctl = nullptr;
static std::shared_ptr<coriander::Parameter> param = nullptr;
}  // namespace

namespace coriander {
namespace zephyr {
void ShellProtocol::enable() {
  ctl = mBoardCtl;
  param = mParam;
}
void ShellProtocol::disable() {
  ctl = nullptr;
  param = nullptr;
}
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

static int shell_calibrate_motor(const struct shell* shell, size_t argc,
                                 char** argv) {
  if (!ctl) {
    return -1;
  }
  ctl->motorCalibrate();
  return 0;
}

static int shell_param_list(const struct shell* shell, size_t argc,
                            char** argv) {
  if (!ctl || !param) {
    return -1;
  }

  auto visitor = coriander::base::PropertyVisitorWrapper(
      [shell](const coriander::base::Property& p) {
        shell_print(shell, "%s\t%s", p.name(), p.desc());
      });
  param->accept(&visitor);

  return 0;
}

static int shell_param_get(const struct shell* shell, size_t argc,
                           char** argv) {
  using TypeId = coriander::TypeId;
  if (!param) {
    return -1;
  }

  if (argc != 2) {
    shell_print(shell, "Usage: param get <name>");
    return -EINVAL;
  }

  const char* name = argv[1];
  auto& p = param->get(name);
  auto& v = p.value();
  auto id = coriander::base::TypeHelper::type(v);

  switch (id) {
    case TypeId::Int32:
      shell_print(shell, "%d", std::get<int32_t>(v));
      break;
    case TypeId::Int64:
      shell_print(shell, "%lld", std::get<int64_t>(v));
      break;
    case TypeId::UInt32:
      shell_print(shell, "%u", std::get<uint32_t>(v));
      break;
    case TypeId::UInt64:
      shell_print(shell, "%llu", std::get<uint64_t>(v));
      break;
    case TypeId::Float:
      shell_print(shell, "%f", std::get<float>(v));
      break;
    case TypeId::Double:
      shell_print(shell, "%lf", std::get<double>(v));
      break;
    case TypeId::String:
      shell_print(shell, "%s", std::get<const char*>(v));
      break;
    case TypeId::TripleInt32:
    case TypeId::TripleFloat:
    case TypeId::TripleDouble:
      shell_print(shell, "not support");
      break;
    default:
      shell_print(shell, "invalid");
      return -1;
  }
  return 0;
}

static int shell_param_set(const struct shell* shell, size_t argc,
                           char** argv) {
  using TypeId = coriander::TypeId;
  if (!ctl || !param) {
    return -1;
  }

  if (argc != 3) {
    shell_print(shell, "Usage: param set <name> <value>");
    return -EINVAL;
  }

  const char* name = argv[1];
  const char* value = argv[2];
  auto p = param->get(name);
  auto& v = p.value();
  auto id = coriander::base::TypeHelper::type(v);

  switch (id) {
    case TypeId::Int32:
      std::get<int32_t>(v) = std::atoi(value);
      break;
    case TypeId::Int64:
      std::get<int64_t>(v) = std::atoll(value);
      break;
    case TypeId::UInt32:
      std::get<uint32_t>(v) = std::atoll(value);
      break;
    case TypeId::UInt64:
      std::get<uint64_t>(v) = std::atoll(value);
      break;
    case TypeId::Float:
      std::get<float>(v) = std::atof(value);
      break;
    case TypeId::Double:
      std::get<double>(v) = std::atof(value);
      break;
    case TypeId::String:
    case TypeId::TripleInt32:
    case TypeId::TripleFloat:
    case TypeId::TripleDouble:
      shell_print(shell, "not support");
      return -EINVAL;
    default:
      shell_print(shell, "invalid");
      return -EINVAL;
  }
  param->replace(p);
  ctl->parameterUpdate();
  return 0;
}

static int shell_param_save(const struct shell* shell, size_t argc,
                            char** argv) {
  if (!param) {
    return -1;
  }
  bool res = param->save();
  return res ? 0 : -1;
}

static int shell_param_load(const struct shell* shell, size_t argc,
                            char** argv) {
  if (!param) {
    return -1;
  }
  bool res = param->load();
  return res ? 0 : -1;
}

SHELL_STATIC_SUBCMD_SET_CREATE(
    motor_cmd,
    SHELL_CMD_ARG(enable, NULL, "enable motor", shell_enable_motor, 1, 0),
    SHELL_CMD_ARG(disable, NULL, "disable motor", shell_disable_motor, 1, 0),
    SHELL_CMD_ARG(calibrate, NULL, "calibrate motor", shell_calibrate_motor, 1,
                  0),
    SHELL_SUBCMD_SET_END /* Array terminated. */
);

SHELL_STATIC_SUBCMD_SET_CREATE(
    param_cmd, SHELL_CMD_ARG(list, NULL, "list params", shell_param_list, 1, 0),
    SHELL_CMD_ARG(get, NULL, "get param", shell_param_get, 2, 0),
    SHELL_CMD_ARG(set, NULL, "set param", shell_param_set, 3, 0),
    SHELL_CMD_ARG(save, NULL, "save param", shell_param_save, 1, 0),
    SHELL_CMD_ARG(load, NULL, "load param", shell_param_load, 1, 0),
    SHELL_SUBCMD_SET_END /* Array terminated. */);

SHELL_CMD_REGISTER(motor, &motor_cmd, "motor control", NULL);
SHELL_CMD_REGISTER(param, &param_cmd, "param control", NULL);
}
