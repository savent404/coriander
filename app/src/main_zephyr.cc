/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#include <app_version.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <boost/di.hpp>

#include "app_version.h"  // NOLINT, this is a generated header in build dir
#include "coriander/application/diagnosis.h"
#include "coriander/base/loggerstream.h"
#include "coriander/base/param.h"
#include "coriander/coriander.h"

// backends
#include "coriander/iprotocol_ctl.h"
#include "zephyr/zephyr_appstatus.h"
#include "zephyr/zephyr_diagnosis.h"
#include "zephyr/zephyr_encoder.h"
#include "zephyr/zephyr_logger.h"
#include "zephyr/zephyr_motor.h"
#include "zephyr/zephyr_mutex.h"
#include "zephyr/zephyr_nvs.h"
#include "zephyr/zephyr_persistent_parameter.h"
#include "zephyr/zephyr_phase_current_estimator.h"
#include "zephyr/zephyr_semaphore.h"
#include "zephyr/zephyr_shell_protocol.h"
#include "zephyr/zephyr_systick.h"
#include "zephyr/zephyr_thread.h"

LOG_MODULE_REGISTER(main);

namespace {

static void loop_hook() { k_sleep(K_MSEC(1)); }

static auto zephyr_backends_bindings() {
  using ILogger = coriander::base::ILogger;
  using IAppStatus = coriander::application::IAppStatus;
  using FocMotorDriver = coriander::motorctl::FocMotorDriver;
  using FocMotorDriverBase = coriander::motorctl::FocMotorDriverBase;
  using IBldcDriver = coriander::motorctl::IBldcDriver;
  using IEncoder = coriander::motorctl::IEncoder;
  using ISystick = coriander::os::ISystick;
  using ISemaphore = coriander::os::ISemaphore;
  using IThread = coriander::os::IThread;
  using IMutex = coriander::os::IMutex;
  using IShellCtl = coriander::IShellCtl;
  using BackendAppStatus = coriander::application::zephyr::AppStatus;
  using BackendMotorDriver = coriander::motorctl::zephyr::MotorDriver;
  // using BackendMotorDriver = coriander::motorctl::zephyr::BldcDriver;
  using IPhaseCurrentEstimator = coriander::motorctl::IPhaseCurrentEstimator;
  using PhaseCurrentEstimator =
      coriander::motorctl::zephyr::PhaseCurrentEstimator;
  using coriander::Parameter;
  using PersistentParameter = coriander::zephyr::PersistentParameter;
  return boost::di::make_injector(
      boost::di::bind<ILogger>.to<coriander::base::zephyr::Logger>(),
      boost::di::bind<IAppStatus>.to<BackendAppStatus>(),
      boost::di::bind<FocMotorDriver>.to<BackendMotorDriver>(),
      boost::di::bind<IBldcDriver>.to<BackendMotorDriver>(),
      boost::di::bind<IEncoder>.to<coriander::motorctl::zephyr::Encoder>(),
      boost::di::bind<ISystick>.to<coriander::os::zephyr::Systick>(),
      boost::di::bind<ISemaphore>.to<coriander::os::zephyr::Semaphore>(),
      boost::di::bind<IThread>.to<coriander::os::zephyr::Thread>(),
      boost::di::bind<IMutex>.to<coriander::os::zephyr::Mutex>(),
      boost::di::bind<IPhaseCurrentEstimator>.to<PhaseCurrentEstimator>(),
      boost::di::bind<Parameter>.to<PersistentParameter>(),
      boost::di::bind<IShellCtl>.to<coriander::zephyr::ShellProtocol>());
}

static int get_reboot_times() {
  auto nvs = coriander::zephyr::nvs::getInstance();
  int reboot_times = 0;
  bool rc = nvs->read(0, &reboot_times, sizeof(reboot_times));

  if (!rc || reboot_times < 0) {
    LOG_WRN("Unable to read reboot times from nvs, reset to 0");
    reboot_times = 0;
  } else {
    reboot_times++;
  }

  rc = nvs->write(0, &reboot_times, sizeof(reboot_times));
  if (!rc) {
    LOG_ERR("Unable to write reboot times to nvs");
    return -1;
  }

  return reboot_times;
}

static void parameter_default_value_setup(
    std::shared_ptr<coriander::Parameter> param) {
  using Property = coriander::base::Property;
  using ID = coriander::base::ParamId;
  using P = Property;

  static const Property properties[] = {
      P{0, ID::MotorCtl_General_Mode_RT},
      P{1, ID::MotorCtl_MotorDriver_PolePair},
      P{0.0f, ID::MotorCtl_MotorDriver_PersistRawElecAngle},
      P{0.0f, ID::MotorCtl_MotorDriver_PersistRawMechAngle},
      P{0.0f, ID::MotorCtl_Calibrate_CaliMechAngleOffset},
      P{3.0f, ID::MotorCtl_Calibrate_CaliVoltage},
      P{3000, ID::MotorCtl_Calibrate_CaliDuration},
      P{24.0f, ID::MotorCtl_MotorDriver_SupplyVoltage},
      P{0.0f, ID::MotorCtl_PosCtl_PidP},
      P{0.0f, ID::MotorCtl_PosCtl_PidI},
      P{0.0f, ID::MotorCtl_PosCtl_PidD},
      P{0.0f, ID::MotorCtl_PosCtl_PidOutputRamp},
      P{0.0f, ID::MotorCtl_PosCtl_PidLimit},
      P{2500, ID::MotorCtl_PosCtl_Freq},
      P{0.0f, ID::MotorCtl_General_TargetPosition_RT},
      P{0.0f, ID::MotorCtl_General_TargetVelocity_RT},
      P{0.0f, ID::MotorCtl_SpeedCtl_PidP},
      P{0.0f, ID::MotorCtl_SpeedCtl_PidI},
      P{0.0f, ID::MotorCtl_SpeedCtl_PidD},
      P{0.0f, ID::MotorCtl_SpeedCtl_PidOutputRamp},
      P{0.0f, ID::MotorCtl_SpeedCtl_PidLimit},
      P{10000, ID::MotorCtl_SpeedCtl_Freq},
      P{16, ID::MotorCtl_SpeedEstimator_WindowSize},
      P{1000, ID::MotorCtl_SpeedEstimator_MinDuration},
      P{10, ID::MotorCtl_SpeedEstimator_SampleInterval},
      P{0, ID::Sensor_Mech_Position_RT},
      P{0, ID::Sensor_Mech_Velocity_RT},
      P{0, ID::Sensor_Motor_Current_RT},
      P{0, ID::Sensor_Motor_Temp_RT},
      P{0, ID::Sensor_Motor_Voltage_RT},
      P{1e-3f, ID::MotorCtl_CurrCtl_Lpf_TimeConstant},    // 1KHz
      P{10e-3f, ID::MotorCtl_SpeedCtl_Lpf_TimeConstant},  // 100KHz
      P{100e-3f, ID::MotorCtl_PosCtl_Lpf_TimeConstant},   // 10Hz
      P(2.0f, ID::MotorCtl_OpenLoop_OutVoltage),          // 2.0V
      P{0.01f, ID::MotorCtl_CurrCtl_PidP},
      P{0.0f, ID::MotorCtl_CurrCtl_PidI},
      P{0.0f, ID::MotorCtl_CurrCtl_PidD},
      P(100.0f, ID::MotorCtl_CurrCtl_PidOutputRamp),
      P{2.5f, ID::MotorCtl_CurrCtl_PidLimit},
      P{4000, ID::MotorCtl_CurrCtl_Freq},
      P{0.0f, ID::MotorCtl_General_TargetCurrentD_RT},
      P{0.0f, ID::MotorCtl_General_TargetCurrentQ_RT},
      P{1000.0f, ID::MotorCtl_CurrCtl_Lpf_TimeConstant},  // 1000us, LPF: 1MHz;
  };
  for (auto& p : properties) {
    param->add(p);
  }
}

template <typename T>
static void zephyr_backend_setup(T* injector) {
  auto os = coriander::base::LoggerStream(
      injector->template create<std::shared_ptr<coriander::base::ILogger>>());
  os << "Coriander version " << APP_VERSION_STRING << std::endl;
  os << "Reboot times: " << get_reboot_times() << std::endl;

  auto param =
      injector->template create<std::shared_ptr<coriander::Parameter>>();
  parameter_default_value_setup(param);

  auto diag_regsiter =
      coriander::application::zephyr::DiagnosisRegister::getInstance();
  auto diag = injector->template create<
      std::shared_ptr<coriander::application::Diagnosis>>();
  diag_regsiter->applyAll(diag.get());
}

}  // namespace

int main(void) {
  auto injector =
      coriander::coriander_create_injector(zephyr_backends_bindings());

  zephyr_backend_setup(&injector);

  coriander::coriander_loop(loop_hook, std::move(injector));
  return 0;
}
