/**
 * @file diagnosis.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief diagnosis interface
 * @date 2023-07-31
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once

#include <array>
#include <cstdint>
#include <forward_list>
#include <functional>

namespace coriander {
namespace application {

struct Diagnosis {
  enum class DeviceStatus : std::uint8_t {
    Normal,
    Warning,
    Error,
    Unknown,
  };
  enum class DiagDevId : std::uint8_t {
    CtlrBoardPsVoltage,
    CtlrBoardPsCurrent,
    CtlrBoardPsPower,
    CtrlBoardTemp,
    CtrlBoardUpperCommunication,
    CtrlBoardP2pCommunication,
    CtrlBoardLowerCommunication,
    DrvBoardTemp,
    MAX_ID,
  };
  using DiagDevFunc = std::function<DeviceStatus()>;
  using DiagInspectorFunc = std::function<void(DiagDevId, DeviceStatus)>;

  Diagnosis() noexcept;
  void setDiagDev(DiagDevId id, DiagDevFunc func) noexcept;
  void addDiagInspector(DiagInspectorFunc func) noexcept;
  void inspect() noexcept;

 protected:
  struct DiagDev {
    DiagDevFunc func;
    DeviceStatus status;
  };
  std::array<DiagDev, static_cast<std::uint8_t>(DiagDevId::MAX_ID)> mDiagDevs;
  std::forward_list<DiagInspectorFunc> mDiagInspectorFuncs;
};

}  // namespace application
}  // namespace coriander
