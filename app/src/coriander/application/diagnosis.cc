/**
 * @file diagnosis.cc
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief 
 * @date 2023-08-18
 * 
 * Copyright 2023 savent_gate
 * 
 */
#include "coriander/application/diagnosis.h"

namespace coriander {
namespace application {

Diagnosis::Diagnosis() noexcept {
  for (auto &dev : mDiagDevs) {
    dev.func = nullptr;
    dev.status = DeviceStatus::Unknown;
  }
}

void Diagnosis::setDiagDev(DiagDevId id, DiagDevFunc func) noexcept {
  if (static_cast<std::uint8_t>(id) <
      static_cast<std::uint8_t>(DiagDevId::MAX_ID)) {
    mDiagDevs[static_cast<std::uint8_t>(id)].func = func;
  }
}

void Diagnosis::addDiagInspector(DiagInspectorFunc func) noexcept {
  mDiagInspectorFuncs.push_front(func);
}

void Diagnosis::inspect() noexcept {
  for (auto &dev : mDiagDevs) {
    if (!dev.func) {
      continue;
    }
    auto status = dev.func();
    if (dev.status == status) {
      continue;
    }
    dev.status = status;
    auto id = static_cast<DiagDevId>(&dev - &mDiagDevs[0]);
    for (auto &inspector : mDiagInspectorFuncs) {
      inspector(id, status);
    }
  }
}

}  // namespace application
}  // namespace coriander
