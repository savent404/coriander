/**
 * @file zephyr_motor.cc
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-09-02
 *
 * Copyright 2023 savent_gate
 *
 */

#include "zephyr/zephyr_motor.h"

#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "zephyr/device.h"

LOG_MODULE_REGISTER(bldc, CONFIG_APP_LOG_LEVEL);

#define SERVO_DEVICE DT_NODELABEL(servo)

struct bldc_data {
  const struct pwm_dt_spec phase[6];
  const struct pwm_dt_spec adc_sample_trigger;
  const struct gpio_dt_spec enable_pin;
  const struct gpio_dt_spec break_pin;
  const uint32_t pwm_deadband_min;
  const uint32_t pwm_deadband_max;
};

static bldc_data bldc_instance = {
    .phase = {
            PWM_DT_SPEC_GET_BY_IDX_OR(SERVO_DEVICE, 0, NULL),  // phase U
            PWM_DT_SPEC_GET_BY_IDX_OR(SERVO_DEVICE, 1, NULL),  // phase V
            PWM_DT_SPEC_GET_BY_IDX_OR(SERVO_DEVICE, 2, NULL),  // phase W
            PWM_DT_SPEC_GET_BY_IDX_OR(SERVO_DEVICE, 3, NULL),  // phase U-N
            PWM_DT_SPEC_GET_BY_IDX_OR(SERVO_DEVICE, 4, NULL),  // phase V-N
            PWM_DT_SPEC_GET_BY_IDX_OR(SERVO_DEVICE, 5, NULL),  // phase W-N
        },
    .adc_sample_trigger =
        PWM_DT_SPEC_GET_BY_NAME_OR(SERVO_DEVICE, adc_trigger, NULL),
    .enable_pin = GPIO_DT_SPEC_GET(SERVO_DEVICE, enable_pin),
    .break_pin = GPIO_DT_SPEC_GET_OR(SERVO_DEVICE, break_pins, {0}),
    .pwm_deadband_min = DT_PROP(SERVO_DEVICE, min_pulse),
    .pwm_deadband_max = DT_PROP(SERVO_DEVICE, max_pulse),
};

static_assert(ARRAY_SIZE(bldc_instance.phase) == 3 ||
                  ARRAY_SIZE(bldc_instance.phase) == 6,
              "BLDC driver requires 3/6 PWM channels");

static int bldc_init() {
  auto inst = &bldc_instance;
  int ret;

  ret = gpio_pin_configure_dt(&inst->enable_pin, GPIO_OUTPUT_INACTIVE);
  if (ret < 0) {
    LOG_ERR("Failed to configure enable pin");
    return ret;
  }

  if (gpio_is_ready_dt(&inst->break_pin)) {
    ret = gpio_pin_configure_dt(&inst->break_pin, GPIO_OUTPUT_INACTIVE);
    if (ret < 0) {
      LOG_ERR("Failed to configure break pin");
      return ret;
    }
  }

  for (size_t i = 0; i < ARRAY_SIZE(inst->phase); i++) {
    ret = pwm_set_pulse_dt(&inst->phase[i], 0);
    if (ret < 0) {
      LOG_ERR("Failed to set PWM pulse");
      return ret;
    }
  }

  ret = pwm_set_pulse_dt(&inst->adc_sample_trigger, 0);
  if (ret < 0) {
    LOG_ERR("Failed to set PWM pulse");
    return ret;
  }

  return 0;
}

SYS_INIT(bldc_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

namespace coriander {
namespace motorctl {
namespace zephyr {

void MotorDriver::enable() {
  auto inst = &bldc_instance;
  int cr4_delay_in_ns = 500;
  // set PWM at 50% duty cycle
  for (size_t i = 0; i < 3; i++) {
    pwm_set_pulse_dt(&inst->phase[i], inst->phase[i].period / 2);
  }

  // disable break
  if (gpio_is_ready_dt(&inst->break_pin)) {
    gpio_pin_set_dt(&inst->break_pin, 0);
  }
  // TODO(savent): based on parameter to adjust TIM1CH4's CCR value
  // NOTE(savent): CCR4 must be greater than ZERO to raise ADC sample trigger
  assert_param(cr4_delay_in_ns > 0);
  pwm_set_pulse_dt(&inst->adc_sample_trigger, PWM_NSEC(cr4_delay_in_ns));
  // enable output
  gpio_pin_set_dt(&inst->enable_pin, PWM_USEC(cr4_delay_in_ns));
}

void MotorDriver::disable() {
  auto inst = &bldc_instance;
  // disable output
  gpio_pin_set_dt(&inst->enable_pin, 0);
  // set PWM at 0% duty cycle
  for (size_t i = 0; i < 3; i++) {
    pwm_set_pulse_dt(&inst->phase[i], 0);
  }
}

void MotorDriver::emergencyBreak() {
  auto inst = &bldc_instance;
  // enable break
  gpio_pin_set_dt(&inst->break_pin, 1);
  // normal disable routine
  disable();
}

static inline uint32_t convert2pulse(uint16_t dutycycle_in_16bit,
                                     uint32_t period, uint32_t min_pulse,
                                     uint32_t max_pulse) {
  uint64_t alpha = dutycycle_in_16bit;
  uint32_t pulse;

  // NOTE(savent): pulse = dutycycle_in_16bit * period / UINT16_MAX
  alpha = alpha * period;
  pulse = alpha >> 16;

  if (pulse < min_pulse) {
    pulse = min_pulse;
  } else if (pulse > max_pulse) {
    pulse = max_pulse;
  }
  return pulse;
}

void MotorDriver::setPhaseDutyCycle(uint16_t u, uint16_t v, uint16_t w) {
  auto inst = &bldc_instance;
  uint32_t pulse[3] = {
      convert2pulse(u, inst->phase[0].period, inst->pwm_deadband_min,
                    inst->pwm_deadband_max),
      convert2pulse(v, inst->phase[1].period, inst->pwm_deadband_min,
                    inst->pwm_deadband_max),
      convert2pulse(w, inst->phase[2].period, inst->pwm_deadband_min,
                    inst->pwm_deadband_max)};
  for (size_t i = 0; i < 3; i++) {
    pwm_set_pulse_dt(&inst->phase[i], pulse[i]);
  }
}

}  // namespace zephyr
}  // namespace motorctl
}  // namespace coriander
