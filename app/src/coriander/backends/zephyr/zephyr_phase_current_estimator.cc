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
#include <stm32f4xx_ll_adc.h>
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

static ADC_TypeDef *adc_from_spec(const adc_dt_spec *spec) {
  // NOTE(savent): based on adc_stm32.c the first four bytes of dev->config is
  // device base addr
  uint32_t base_addr = *(uint32_t *)(spec->dev->config);
  switch (base_addr) {
    case ADC1_BASE:
      return ADC1;
    case ADC2_BASE:
      return ADC2;
    case ADC3_BASE:
      return ADC3;
    default:
      break;
  }
  return NULL;
}

static uint32_t channel_from_idx(unsigned idx) {
  constexpr const uint32_t channels[] = {
      LL_ADC_CHANNEL_0,  LL_ADC_CHANNEL_1,  LL_ADC_CHANNEL_2,
      LL_ADC_CHANNEL_3,  LL_ADC_CHANNEL_4,  LL_ADC_CHANNEL_5,
      LL_ADC_CHANNEL_6,  LL_ADC_CHANNEL_7,  LL_ADC_CHANNEL_8,
      LL_ADC_CHANNEL_9,  LL_ADC_CHANNEL_10, LL_ADC_CHANNEL_11,
      LL_ADC_CHANNEL_12, LL_ADC_CHANNEL_13, LL_ADC_CHANNEL_14,
      LL_ADC_CHANNEL_15, LL_ADC_CHANNEL_16, LL_ADC_CHANNEL_17,
      LL_ADC_CHANNEL_18,
  };
  if (idx >= ARRAY_SIZE(channels)) {
    return 0;
  }
  return channels[idx];
}

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

  // NOTE(savent): Low-Level configuration
  ADC_TypeDef *adc_first = NULL, *adc_second = NULL;
  for (int i = 0; i < 3; i++) {
    ADC_TypeDef *adc = adc_from_spec(&adc_channels[i]);
    uint32_t adc_channel = channel_from_idx(adc_channels[i].channel_id);

    LL_ADC_INJ_SetTrigAuto(adc, LL_ADC_INJ_TRIG_INDEPENDENT);
    LL_ADC_INJ_SetSequencerDiscont(adc, LL_ADC_INJ_SEQ_DISCONT_DISABLE);
    LL_ADC_INJ_SetTriggerSource(adc, LL_ADC_INJ_TRIG_EXT_TIM1_CH4);
    LL_ADC_INJ_SetSequencerLength(adc, LL_ADC_INJ_SEQ_SCAN_DISABLE);

    LL_ADC_INJ_SetSequencerRanks(adc, LL_ADC_INJ_RANK_1, adc_channel);
    // Don't re-configure the channel's sampling time, it's already done by
    // zephyr
    LL_ADC_INJ_SetOffset(adc, LL_ADC_INJ_RANK_1, 0);
    LL_ADC_DisableIT_JEOS(adc);

    LL_ADC_INJ_StartConversionExtTrig(adc, LL_ADC_INJ_TRIG_EXT_RISING);
    // parameter check
    if (adc == adc_first || adc == adc_second) {
      LOG_ERR(
          "phase current must use different ADCs to sample at the same "
          "time!\n");
      k_panic();
    }
    if (i == 0) {
      adc_first = adc;
    } else if (i == 1) {
      adc_second = adc;
    }
  }
}

static void adc_sync(adc_instance *inst) {
  constexpr const adc_dt_spec *adc_channels = &adc_inst.adc_channels[0];
  constexpr int32_t *adc_raw = &adc_inst.adc_raw[0];
  for (int i = 0; i < 3; i++) {
    ADC_TypeDef *adc = adc_from_spec(&adc_channels[i]);
    uint32_t adc_rank = LL_ADC_INJ_RANK_1;
    uint32_t val;
    val = LL_ADC_INJ_ReadConversionData12(adc, adc_rank);
    adc_raw[i] = static_cast<int32_t>((val * 3.3f / 4096) * 1000.0f);
  }
  LOG_INF("raw message--Iu: %d, Iv: %d, Iw: %d\n", adc_raw[0], adc_raw[1],
          adc_raw[2]);
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
