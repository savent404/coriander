/**
 * @file appstatus.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-07-27
 *
 * Copyright 2023 savent_gate
 *
 */
#include "coriander/application/appstatus.h"
#include <zephyr/drivers/led.h>
#include <zephyr/kernel.h>

#define APP_LED DT_CHOSEN(usr_led)
#define APP_LED_DEVICE DEVICE_DT_GET(APP_LED)

#define APP_LED_BLINK_PERIOD 250

namespace {
extern "C" {

using Status = coriander::AppStatus::Status;
static Status s_status = Status::Busy;
static int s_on = 0;

static void AppStatusPeriodCallback(struct k_timer* timer)
{
    int on = 1;

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
} // namespace

namespace coriander {

AppStatus::AppStatus() noexcept
{
    led_off(APP_LED_DEVICE, 0);
    k_timer_start(&app_status, K_MSEC(APP_LED_BLINK_PERIOD), K_MSEC(APP_LED_BLINK_PERIOD));
}

void AppStatus::setStatus(Status status) noexcept
{
    s_status = status;
    led_on(APP_LED_DEVICE, 0);
}

AppStatus::Status AppStatus::getStatus() const noexcept
{
    return s_status;
}

}
