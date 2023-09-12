/**
 * @file zephyr_logger.cc
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-07-31
 *
 * Copyright 2023 savent_gate
 *
 */

#include "zephyr/zephyr_logger.h"

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(c, LOG_LEVEL_INF);
namespace coriander {
namespace base {
namespace zephyr {

void Logger::log(const char *msg) noexcept { LOG_INF("%s", msg); }

}  // namespace zephyr
}  // namespace base
}  // namespace coriander
