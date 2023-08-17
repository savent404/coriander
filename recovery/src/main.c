/**
 * @file main.c
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-16
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <zephyr/drivers/led.h>
#include <zephyr/kernel.h>

#define APP_LED DT_CHOSEN(usr_led)
#define APP_LED_DEVICE DEVICE_DT_GET_OR_NULL(APP_LED)

int main(void) {
  int t = 0;
  printk("Hello World! %s\n", CONFIG_BOARD);

  while (1) {
    if (t & 1) {
      led_on(APP_LED_DEVICE, 0);
    } else {
      led_off(APP_LED_DEVICE, 0);
    }
    k_msleep(200);
    t++;
  }
  return 0;
}
