/**
 * @file zephyr_diagnosis.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-07-31
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once

#include <cstddef>

#include "coriander/application/diagnosis.h"

namespace coriander {
namespace application {
namespace zephyr {
struct DiagnosisRegister {
  using DeviceStatus = coriander::application::Diagnosis::DeviceStatus;
  using DiagDevId = coriander::application::Diagnosis::DiagDevId;
  DiagnosisRegister(const DiagnosisRegister& other) = delete;
  DiagnosisRegister& operator=(const DiagnosisRegister& other) = delete;
  ~DiagnosisRegister() = delete;

  DiagnosisRegister();
  void applyAll(coriander::application::Diagnosis& diagnosis);
  void updateStatus(uint32_t ref_2048, uint32_t ps28Voltage,
                    uint32_t ps5Voltage, uint32_t psCurrent);
  static DiagnosisRegister* getInstance();

 private:
  DiagnosisRegister* mInstance;
  DeviceStatus mPsVoltage, mPsCurrent;
};
}  // namespace zephyr
}  // namespace application
}  // namespace coriander
