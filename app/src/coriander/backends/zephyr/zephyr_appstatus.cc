/**
 * @file appstatus.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-07-27
 *
 * Copyright 2023 savent_gate
 *
 */
#include <zephyr/drivers/led.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "zephyr/zephyr_appstatus.h"

LOG_MODULE_REGISTER(appstatus, 2);

#define APP_LED DT_CHOSEN(usr_led)
#define APP_LED_DEVICE DEVICE_DT_GET_OR_NULL(APP_LED)

#define APP_LED_BLINK_PERIOD 250

namespace {
extern "C" {

using Status = coriander::application::zephyr::AppStatus::Status;
static Status s_status = Status::Busy;
static int s_on = 0;

static bool has_led() {
  if (!APP_LED_DEVICE || !device_is_ready(APP_LED_DEVICE)) {
    return false;
  }
  return true;
}

static void AppStatusPeriodCallback(struct k_timer* timer) {
  int on = 1;

  if (!has_led()) {
    return;
  }

  switch (s_status) {
    case Status::Ok:
      on = s_on & 4;
      break;
    case Status::Busy:
      on = s_on & 1;
      break;
    case Status::Error:
      on = 1;
      break;
  }

  if (on) {
    led_on(APP_LED_DEVICE, 0);
  } else {
    led_off(APP_LED_DEVICE, 0);
  }

  s_on++;
}

K_TIMER_DEFINE(app_status, AppStatusPeriodCallback, NULL);
}
}  // namespace

namespace coriander {
namespace application {

namespace zephyr {
AppStatus::AppStatus() noexcept {
  if (!has_led()) {
    LOG_ERR("LED device not found");
    return;
  }
  led_off(APP_LED_DEVICE, 0);
  k_timer_start(&app_status, K_MSEC(APP_LED_BLINK_PERIOD),
                K_MSEC(APP_LED_BLINK_PERIOD));
}

void AppStatus::setStatus(Status status) noexcept {
  if (!has_led()) {
    return;
  }
  s_status = status;
  led_on(APP_LED_DEVICE, 0);
}

AppStatus::Status AppStatus::getStatus() const noexcept { return s_status; }

}  // namespace zephyr
}  // namespace application
}  // namespace coriander
