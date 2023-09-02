/**
 * @file zephyr_diagnosis.cc
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-18
 *
 * Copyright 2023 savent_gate
 *
 */

#include "zephyr/zephyr_diagnosis.h"

#include <zephyr/drivers/adc.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(diagnosis);

#ifdef CONFIG_CORIANDER_MINIMAL
namespace coriander {
namespace application {
namespace zephyr {
DiagnosisRegister::DiagnosisRegister() {}
DiagnosisRegister* DiagnosisRegister::getInstance() {
  static DiagnosisRegister* instance;
  if (instance == nullptr) {
    instance = new DiagnosisRegister();
  }
  return instance;
}
void DiagnosisRegister::applyAll(coriander::application::Diagnosis* diagnosis) {
}
void DiagnosisRegister::updateStatus(uint32_t ref_2048, uint32_t ps28Voltage,
                                     uint32_t ps5Voltage, uint32_t psCurrent) {}
}  // namespace zephyr
}  // namespace application
}  // namespace coriander
#else

#define DT_SPEC_AND_COMMA(node_id, prop, idx) \
  ADC_DT_SPEC_GET_BY_IDX(node_id, idx),

using DeviceStatus = coriander::application::Diagnosis::DeviceStatus;
using DiagDevId = coriander::application::Diagnosis::DiagDevId;

/* Data of ADC io-channels specified in devicetree. */
static const struct adc_dt_spec adc_channels[] = {
    DT_FOREACH_PROP_ELEM(DT_PATH(zephyr_user), io_channels, DT_SPEC_AND_COMMA)};
static void diagnosis_thread(void) {
  int err;
  uint16_t buf;
  uint32_t adc_raw[ARRAY_SIZE(adc_channels)];
  struct adc_sequence sequence = {
      .buffer = &buf,
      /* buffer size in bytes, not number of samples */
      .buffer_size = sizeof(buf),
  };

  /* Configure channels individually prior to sampling. */
  for (size_t i = 0U; i < ARRAY_SIZE(adc_channels); i++) {
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

  while (1) {
    for (size_t i = 0U; i < ARRAY_SIZE(adc_channels); i++) {
      int32_t val_mv;

      adc_sequence_init_dt(&adc_channels[i], &sequence);
      err = adc_read(adc_channels[i].dev, &sequence);
      if (err < 0) {
        LOG_ERR("Could not read (%d)\n", err);
        continue;
      }

      /*
       * If using differential mode, the 16 bit value
       * in the ADC sample buffer should be a signed 2's
       * complement value.
       */
      if (adc_channels[i].channel_cfg.differential) {
        val_mv = (int32_t)((int16_t)buf);
      } else {
        val_mv = (int32_t)buf;
      }
      err = adc_raw_to_millivolts_dt(&adc_channels[i], &val_mv);
      /* conversion to mV may not be supported, skip if not */
      if (err < 0) {
        LOG_ERR(" (value in mV not available)\n");
        continue;
      }
      adc_raw[i] = val_mv;
    }

    coriander::application::zephyr::DiagnosisRegister::getInstance()
        ->updateStatus(adc_raw[0], adc_raw[1], adc_raw[2], adc_raw[3]);

    // update diagnosis status
    k_sleep(K_MSEC(1000));
  }
  return;
}

K_THREAD_DEFINE(diagnosis, 1024, diagnosis_thread, NULL, NULL, NULL, 7, 0, -1);

namespace coriander {
namespace application {
namespace zephyr {

DiagnosisRegister::DiagnosisRegister()
    : mInstance(nullptr),
      mPsVoltage(DeviceStatus::Unknown),
      mPsCurrent(DeviceStatus::Unknown) {
  k_thread_start(diagnosis);
}

DiagnosisRegister* DiagnosisRegister::getInstance() {
  static DiagnosisRegister* instance;
  if (instance == nullptr) {
    instance = new DiagnosisRegister();
  }
  return instance;
}

void DiagnosisRegister::applyAll(coriander::application::Diagnosis* diagnosis) {
  using DeviceStatus = coriander::application::Diagnosis::DeviceStatus;
  using DiagDevId = coriander::application::Diagnosis::DiagDevId;

  diagnosis->setDiagDev(DiagDevId::CtlrBoardPsVoltage, [this]() {
    while (mPsVoltage == DeviceStatus::Unknown) {
      k_sleep(K_MSEC(1));
    }
    return mPsVoltage;
  });
  diagnosis->setDiagDev(DiagDevId::CtlrBoardPsCurrent, [this]() {
    while (mPsCurrent == DeviceStatus::Unknown) {
      k_sleep(K_MSEC(1));
    }
    return mPsCurrent;
  });
}

void DiagnosisRegister::updateStatus(uint32_t ref_2048, uint32_t ps28Voltage,
                                     uint32_t ps5Voltage, uint32_t psCurrent) {
  // factor * adc = 2048
  float factor = 1;

  uint32_t ps28VoltageF = (ps28Voltage * factor - 1440) / 0.029f;
  uint32_t ps5VoltageF = ps5Voltage * factor * 2;
  uint32_t psCurrentF = (psCurrent * factor - 1650.0f) / 0.264f;

  LOG_INF("28V: %d, 5V: %d, current(mA): %d", ps28VoltageF, ps5VoltageF,
          psCurrentF);

  if (ps5VoltageF > 6000 || ps5VoltageF < 4000) {
    LOG_WRN("ctrl 5V voltage abnormal");
    mPsVoltage = DeviceStatus::Error;
  } else {
    mPsVoltage = DeviceStatus::Normal;
  }

  if (psCurrentF > 1000) {
    LOG_WRN("ctrl current abnormal");
    mPsCurrent = DeviceStatus::Error;
  } else {
    mPsCurrent = DeviceStatus::Normal;
  }
}

}  // namespace zephyr
}  // namespace application
}  // namespace coriander

#endif
