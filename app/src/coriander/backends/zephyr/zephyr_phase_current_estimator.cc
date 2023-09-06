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

#include "coriander/base/jscope.h"

LOG_MODULE_REGISTER(phase_current_estimator);

#if CONFIG_JSCOPE_ENABLE
#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
ATTR_JSCOPE static float _dPhaseCurrentU = 0.0f;
ATTR_JSCOPE static float _dPhaseCurrentV = 0.0f;
ATTR_JSCOPE static float _dPhaseCurrentW = 0.0f;
ATTR_JSCOPE static float _dPhaseCurrentAlpha = 0.0f;
ATTR_JSCOPE static float _dPhaseCurrentBeta = 0.0f;
ATTR_JSCOPE static float _dPhaseCurrentAngle = 0.0f;
#endif

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
    .adc_channels = {ADC_DT_SPEC_GET_BY_NAME(DT_PATH(zephyr_user), motor_iu),
                     ADC_DT_SPEC_GET_BY_NAME(DT_PATH(zephyr_user), motor_iv),
                     ADC_DT_SPEC_GET_BY_NAME(DT_PATH(zephyr_user), motor_iw)},
    .adc_raw = {0},
};

static inline ADC_TypeDef *adc_from_spec(const adc_dt_spec *spec) {
  // NOTE(savent): based on adc_stm32.c the first four bytes of dev->config is
  // device base addr
  uint32_t base_addr = *reinterpret_cast<const uint32_t *>(spec->dev->config);
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

static inline uint32_t channel_from_idx(unsigned idx) {
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

static bool adc_inited = false;
static void adc_init(adc_instance *inst) {
  auto &init = adc_inited;
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
static inline void current_convert(adc_instance *inst, float *Iu, float *Iv,
                                   float *Iw) {
  constexpr int32_t offset = PHASE_CURRENT_OFFSET;
  constexpr float factor = (1.0f / PHASE_CURRENT_SCALE);
  int32_t *adc_raw = &inst->adc_raw[0];
  float Ia, Ib, Ic;

  // based on Ia + Ib + Ic = 0
  Ia = (adc_raw[0] - offset) * factor;
  Ib = (adc_raw[1] - offset) * factor;
  Ic = (adc_raw[2] - offset) * factor;

  // assert abs(Ia + Ib + Ic) < 0.01A
  assert_param(Ia + Ib + Ic < 0.01f && Ia + Ib + Ic > -0.01f);

  *Iu = Ia;
  *Iv = Ib;
  *Iw = Ic;
}

static inline void get_current_alpha_beta(adc_instance *inst, const float Ia,
                                          const float Ib, const float Ic,
                                          float *alpha, float *beta) {
  *alpha = Ia - 0.5f * Ib - 0.5f * Ic;
  *beta = 0.866025f * Ib - 0.866025f * Ic;
}

static inline void adc_sync(adc_instance *inst) {
  constexpr const adc_dt_spec *adc_channels = &adc_inst.adc_channels[0];
  constexpr int32_t *adc_raw = &adc_inst.adc_raw[0];
  for (int i = 0; i < 3; i++) {
    ADC_TypeDef *adc = adc_from_spec(&adc_channels[i]);
    uint32_t adc_rank = LL_ADC_INJ_RANK_1;
    uint32_t val;
    val = LL_ADC_INJ_ReadConversionData12(adc, adc_rank);
    adc_raw[i] = static_cast<int32_t>((val * 3.3f / 4096) * 1000.0f);
  }
}

namespace coriander {
namespace motorctl {
namespace zephyr {
void PhaseCurrentEstimator::enable() { adc_init(&adc_inst); }
void PhaseCurrentEstimator::disable() {}
bool PhaseCurrentEstimator::enabled() { return adc_inited; }
void PhaseCurrentEstimator::sync() {
  adc_sync(&adc_inst);
#if CONFIG_JSCOPE_ENABLE
  float alpha, beta, angle;
  getPhaseCurrent(&alpha, &beta);
  _dPhaseCurrentAlpha = alpha * 1000;
  _dPhaseCurrentBeta = beta * 1000;
  angle = atan2f(beta, alpha);
  _dPhaseCurrentAngle = angle * 180.0f / M_PI;
#endif
}

void PhaseCurrentEstimator::getPhaseCurrent(float *alpha, float *beta) {
  float c[3];
  current_convert(&adc_inst, &c[0], &c[1], &c[2]);

  if (calibrateFlag) {
    for (int i = 0; i < 3; i++) {
      c[i] -= offset[i];
    }
  }

#if CONFIG_JSCOPE_ENABLE
  _dPhaseCurrentU = c[0] * 1000;
  _dPhaseCurrentV = c[1] * 1000;
  _dPhaseCurrentW = c[2] * 1000;
#endif

  get_current_alpha_beta(&adc_inst, c[0], c[1], c[2], alpha, beta);

  for (int i = 0; i < 3; i++) {
    current[i] = c[i];
  }
}

void PhaseCurrentEstimator::calibrate() {
  float r[3], d[3];
  current_convert(&adc_inst, &r[0], &r[1], &r[2]);

  // assume the current is stable as zero, c[i] is the offset
  for (int i = 0; i < 3; i++) {
    d[i] = r[i] - offset[i];
  }

  for (int i = 0; i < 3; i++) {
    offset[i] += d[i] * 0.1;  // use 10% of the current to calibrate, callibrate
                              // more than 10 times to get a stable offset
  }
  LOG_INF("raw/diff/offset: %d/%f/%f %d/%f/%f %d/%f/%f", adc_inst.adc_raw[0],
          d[0], offset[0], adc_inst.adc_raw[1], d[1], offset[1],
          adc_inst.adc_raw[2], d[2], offset[2]);
  calibrateFlag = true;
}

bool PhaseCurrentEstimator::needCalibrate() { return !calibrateFlag; }

}  // namespace zephyr
}  // namespace motorctl
}  // namespace coriander
