/**
 * @file zephyr_phase_current_estimator.cc
 * @author savent404 (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-30
 *
 * Copyright 2023 savent_gate
 *
 */
#include "zephyr/zephyr_phase_current_estimator.h"

#include <stddef.h>
#include <stdint.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(phase_current_estimator);

#ifndef ADC_DT_SPEC_GET_BY_NAME
#define ADC_DT_SPEC_GET_BY_NAME(node_id, name)                   \
  ADC_DT_SPEC_STRUCT(DT_IO_CHANNELS_CTLR_BY_NAME(node_id, name), \
                     DT_IO_CHANNELS_INPUT_BY_NAME(node_id, name))
#endif

static const struct adc_dt_spec adc_channels[] = {
    ADC_DT_SPEC_GET_BY_NAME(DT_PATH(zephyr_user), motor_iu),
    ADC_DT_SPEC_GET_BY_NAME(DT_PATH(zephyr_user), motor_iv),
    ADC_DT_SPEC_GET_BY_NAME(DT_PATH(zephyr_user), motor_iw),
};
static int32_t adc_raw[ARRAY_SIZE(adc_channels)];

static_assert(ARRAY_SIZE(adc_channels) == 3U,
              "3 ADC channels are required(Ia, Ib, Ic)");

namespace coriander {
namespace motorctl {
namespace zephyr {
void PhaseCurrentEstimator::enable() {}
void PhaseCurrentEstimator::disable() {}
bool PhaseCurrentEstimator::enabled() {
  bool enabled = true;
  for (size_t i = 0U; i < ARRAY_SIZE(adc_channels); i++) {
    enabled &= adc_is_ready_dt(&adc_channels[i]);
  }
  return enabled;
}
void PhaseCurrentEstimator::sync() {
  static bool adc_configured = false;
  static bool adc_fatal_error = false;
  static uint16_t buf[3];
  static struct adc_sequence sequence[3] = {
      {.buffer = &buf[0], .buffer_size = 2},
      {.buffer = &buf[1], .buffer_size = 2},
      {.buffer = &buf[2], .buffer_size = 2},
  };
  int32_t val;

  if (adc_fatal_error) {
    return;
  }

  if (!adc_configured) {
    for (size_t i = 0U; i < ARRAY_SIZE(adc_channels); i++) {
      if (!adc_is_ready_dt(&adc_channels[i])) {
        LOG_ERR("ADC controller device %s not ready\n",
                adc_channels[i].dev->name);
        adc_fatal_error = true;
        return;
      }

      int err = adc_channel_setup_dt(&adc_channels[i]);
      if (err < 0) {
        LOG_ERR("Could not setup channel #%d (%d)\n", i, err);
        adc_fatal_error = true;
        return;
      }
    }
    adc_configured = true;
  }

  for (size_t i = 0U; i < ARRAY_SIZE(adc_channels); i++) {
    adc_sequence_init_dt(&adc_channels[i], &sequence[i]);
    int err = adc_read(adc_channels[i].dev, &sequence[i]);
    if (err < 0) {
      LOG_ERR("Could not read (%d)\n", err);
    }

    if (adc_channels[i].channel_cfg.differential) {
      val = (int32_t)((int16_t)buf[i]);
    } else {
      val = (int32_t)buf[i];
    }
    err = adc_raw_to_millivolts_dt(&adc_channels[i], &val);
    if (err < 0) {
      LOG_ERR(" (value in mV not available)\n");
      continue;
    }
    adc_raw[i] = val;
  }
}

void PhaseCurrentEstimator::getPhaseCurrent(float *alpha, float *beta) {
  float Ia, Ib, Ic;

  // TODO(savent): need to know current motor sector to recovery Ix
  // based on Ia + Ib + Ic = 0
  Ia = (adc_raw[0] - 1650) / 8.8f;
  Ib = (adc_raw[1] - 1650) / 8.8f;
  Ic = (adc_raw[2] - 1650) / 8.8f;

  *alpha = Ia - 0.5f * Ib - 0.5f * Ic;
  *beta = 0.866025f * Ib - 0.866025f * Ic;
}

void PhaseCurrentEstimator::calibrate() {}
bool PhaseCurrentEstimator::needCalibrate() { return false; }

}  // namespace zephyr
}  // namespace motorctl
}  // namespace coriander
