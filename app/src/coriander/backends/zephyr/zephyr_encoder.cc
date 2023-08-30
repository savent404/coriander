/**
 * @file zephyr_encoder.cc
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-30
 *
 * Copyright 2023 savent_gate
 *
 */
#include "zephyr/zephyr_encoder.h"

#include <zephyr/drivers/pwm.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(encoder, CONFIG_APP_LOG_LEVEL);

#define ENCODER_QDEC DEVICE_DT_GET(DT_CHOSEN(usr_encoder))

struct _instance {
  const struct device* encoder;
  const uint32_t ppr;
  bool found_zero;
  struct sensor_value angle_cached;
  struct sensor_value full_angle;
};

static struct _instance _encoder_instance = {
    .encoder = ENCODER_QDEC,
    .ppr = 0x8000,
    .found_zero = false,
};

namespace coriander {
namespace motorctl {
namespace zephyr {
void Encoder::enable() {}
void Encoder::disable() {}
bool Encoder::enabled() { return true; }
void Encoder::sync() {
  struct _instance* inst = &_encoder_instance;
  int32_t diff, t = inst->angle_cached.val1;
  int ret;

  ret = sensor_sample_fetch(inst->encoder);
  if (ret) {
    LOG_WRN("Failed to fetch sample from %s: %d", inst->encoder->name, ret);
    return;
  }

  ret = sensor_channel_get(inst->encoder, SENSOR_CHAN_ROTATION,
                           &inst->angle_cached);
  if (ret) {
    LOG_WRN("Failed to get angle from %s: %d", inst->encoder->name, ret);
    return;
  }

  diff = inst->angle_cached.val1 - t;

  if (abs(diff) > 180) {
    if (diff < 0) {
      diff += 360;
    } else {
      diff -= 360;
    }
  }

  inst->full_angle.val1 += diff;
}
void Encoder::calibrate() {
  struct _instance* inst = &_encoder_instance;
  inst->found_zero = true;
}
bool Encoder::needCalibrate() {
  struct _instance* inst = &_encoder_instance;
  return !inst->found_zero;
}
unsigned Encoder::getEncoderCount() {
  struct _instance* inst = &_encoder_instance;
  float angle = sensor_value_to_float(&inst->angle_cached);
  return (unsigned)(angle / 360.0 * inst->ppr);
}
unsigned Encoder::getEncoderCountPerRound() {
  struct _instance* inst = &_encoder_instance;
  return inst->ppr;
}
int Encoder::getOverflowCount() {
  struct _instance* inst = &_encoder_instance;
  return inst->full_angle.val1 / 360;
}
}  // namespace zephyr
}  // namespace motorctl
}  // namespace coriander
