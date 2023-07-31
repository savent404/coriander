/**
 * @file ut_diagnosis.cc
 * @author Z004TPFF (yuankai.liao@siemens.com)
 * @brief
 * @date 2023-07-31
 *
 * Copyright 2023 savent_gate
 *
 */
#include <gtest/gtest.h>

#include "coriander/application/diagnosis.h"

TEST(Diagnosis, basic) {
  using Diagnosis = coriander::application::Diagnosis;

  bool called = false;
  Diagnosis diag;
  diag.setDiagDev(Diagnosis::DiagDevId::CtlrBoardPsVoltage,
                  []() { return Diagnosis::DeviceStatus::Normal; });
  diag.addDiagInspector(
      [&](Diagnosis::DiagDevId id, Diagnosis::DeviceStatus status) {
        EXPECT_EQ(id, Diagnosis::DiagDevId::CtlrBoardPsVoltage);
        EXPECT_EQ(status, Diagnosis::DeviceStatus::Normal);
        called = true;
      });

  diag.inspect();
  EXPECT_EQ(called, true);
}

TEST(Diagnosis, trigger) {
  using Diagnosis = coriander::application::Diagnosis;
  using DiagDevId = coriander::application::Diagnosis::DiagDevId;
  bool called_matrix[] = {
      [static_cast<int>(DiagDevId::CtlrBoardPsVoltage)] = false,
      [static_cast<int>(DiagDevId::CtlrBoardPsCurrent)] = false,
      [static_cast<int>(DiagDevId::CtlrBoardPsPower)] = false,
      [static_cast<int>(DiagDevId::CtrlBoardTemp)] = false,
  };

  Diagnosis diag;

  diag.setDiagDev(DiagDevId::CtlrBoardPsVoltage,
                  []() { return Diagnosis::DeviceStatus::Normal; });
  diag.setDiagDev(DiagDevId::CtlrBoardPsCurrent,
                  []() { return Diagnosis::DeviceStatus::Warning; });
  diag.setDiagDev(DiagDevId::CtlrBoardPsPower,
                  []() { return Diagnosis::DeviceStatus::Error; });
  diag.setDiagDev(DiagDevId::CtrlBoardTemp,
                  []() { return Diagnosis::DeviceStatus::Unknown; });

  diag.addDiagInspector(
      [&](Diagnosis::DiagDevId id, Diagnosis::DeviceStatus status) {
        called_matrix[static_cast<int>(id)] = true;
      });

  diag.inspect();

  EXPECT_EQ(called_matrix[static_cast<int>(DiagDevId::CtlrBoardPsVoltage)],
            true);
  EXPECT_EQ(called_matrix[static_cast<int>(DiagDevId::CtlrBoardPsCurrent)],
            true);
  EXPECT_EQ(called_matrix[static_cast<int>(DiagDevId::CtlrBoardPsPower)], true);
  EXPECT_EQ(called_matrix[static_cast<int>(DiagDevId::CtrlBoardTemp)], false);
}
