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

#define PHASE_CURRENT_NODE DT_NODELABEL(phase_current)
#define PHASE_CURRENT_OFFSET DT_PROP(PHASE_CURRENT_NODE, voltage_offset)
#define PHASE_CURRENT_SCALE DT_PROP(PHASE_CURRENT_NODE, voltage_scale)

struct adc_instance {
  const struct adc_dt_spec adc_channels[3];
  int32_t adc_raw[3];
};

static struct adc_instance adc_inst = {
    .adc_channels =
        {
            ADC_DT_SPEC_GET_BY_NAME(DT_PATH(zephyr_user), motor_iu),
            ADC_DT_SPEC_GET_BY_NAME(DT_PATH(zephyr_user), motor_iv),
            ADC_DT_SPEC_GET_BY_NAME(DT_PATH(zephyr_user), motor_iw),
        },
    .adc_raw = {0},
};

static void adc_init(adc_instance *inst) {
  static bool init = false;
  const adc_dt_spec *adc_channels = &inst->adc_channels[0];
  int err;
  if (init) {
    return;
  }
  init = true;

  for (int i = 0; i < 3; i++) {
    if (!adc_is_ready_dt(&adc_channels[i])) {
      LOG_ERR("ADC controller device %s not ready\n",
              adc_channels[i].dev->name);
      return;
    }
    err = adc_channel_setup_dt(&adc_channels[i]);
    if (err < 0) {
      LOG_ERR("Could not setup channel #%d (%d)\n", i, err);
      return;
    }
  }
}
static void adc_sync(adc_instance *inst) {
  static bool adc_fatal_error = false;
  constexpr const adc_dt_spec *adc_channels = &adc_inst.adc_channels[0];
  constexpr int32_t *adc_raw = &adc_inst.adc_raw[0];
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

  for (size_t i = 0U; i < 3; i++) {
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
  LOG_INF("raw message--Iu: %d, Iv: %d, Iw: %d\n", adc_raw[0], adc_raw[1], adc_raw[2]);
}

namespace coriander {
namespace motorctl {
namespace zephyr {
void PhaseCurrentEstimator::enable() { adc_init(&adc_inst); }
void PhaseCurrentEstimator::disable() {}
bool PhaseCurrentEstimator::enabled() {
  constexpr const adc_dt_spec *adc_channels = &adc_inst.adc_channels[0];
  bool enabled = true;
  for (size_t i = 0U; i < 3; i++) {
    enabled &= adc_is_ready_dt(&adc_channels[i]);
  }
  return enabled;
}
void PhaseCurrentEstimator::sync() { adc_sync(&adc_inst); }

void PhaseCurrentEstimator::getPhaseCurrent(float *alpha, float *beta) {
  constexpr float offset = PHASE_CURRENT_OFFSET;
  constexpr float factor = (1000.0f / PHASE_CURRENT_SCALE);
  constexpr int32_t *adc_raw = &adc_inst.adc_raw[0];
  float Ia, Ib, Ic;

  // based on Ia + Ib + Ic = 0
  Ia = (adc_raw[0] - offset) * factor;
  Ib = (adc_raw[1] - offset) * factor;
  Ic = (adc_raw[2] - offset) * factor;

  *alpha = Ia - 0.5f * Ib - 0.5f * Ic;
  *beta = 0.866025f * Ib - 0.866025f * Ic;
}

void PhaseCurrentEstimator::calibrate() {}
bool PhaseCurrentEstimator::needCalibrate() { return false; }

}  // namespace zephyr
}  // namespace motorctl
}  // namespace coriander
