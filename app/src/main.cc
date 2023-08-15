/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#include <app_version.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <boost/di.hpp>

#include "app_version.h"
#include "coriander/application/diagnosis.h"
#include "coriander/base/loggerstream.h"
#include "coriander/base/param.h"
#include "coriander/coriander.h"

// backends
#include "zephyr_appstatus.h"
#include "zephyr_diagnosis.h"
#include "zephyr_encoder.h"
#include "zephyr_logger.h"
#include "zephyr_motor.h"
#include "zephyr_mutex.h"
#include "zephyr_nvs.h"
#include "zephyr_semaphore.h"
#include "zephyr_shell_protocol.h"
#include "zephyr_systick.h"
#include "zephyr_thread.h"

LOG_MODULE_REGISTER(main);

namespace {

static void loop_hook() { k_sleep(K_MSEC(1)); }

using namespace boost;
static auto zephyr_backends_bindings() {
  return di::make_injector(
      di::bind<coriander::base::ILogger>.to<coriander::base::zephyr::Logger>(),
      di::bind<coriander::application::IAppStatus>.to<coriander::application::zephyr::AppStatus>(),
      di::bind<coriander::motorctl::FocMotorDriver>.to<coriander::motorctl::zephyr::MotorDriver>(),
      di::bind<coriander::motorctl::IBldcDriver>.to<coriander::motorctl::zephyr::MotorDriver>(),
      di::bind<coriander::motorctl::IEncoder>.to<coriander::motorctl::zephyr::Encoder>(),
      di::bind<coriander::os::ISystick>.to<coriander::os::zephyr::Systick>(),
      di::bind<coriander::os::ISemaphore>.to<coriander::os::zephyr::Semaphore>(),
      di::bind<coriander::os::IThread>.to<coriander::os::zephyr::Thread>(),
      di::bind<coriander::os::IMutex>.to<coriander::os::zephyr::Mutex>(),
      di::bind<coriander::IProtocolCtl>.to<coriander::zephyr::ShellProtocol>());
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
    std::shared_ptr<coriander::ParameterBase> param) {
  using Property = coriander::base::Property;
  using ID = coriander::base::ParamId;
  using P = Property;

  const static Property properties[] = {
      P{0, ID::MotorCtlMode},
      P{1, ID::PolePair},
      P{0.0f, ID::ElecAngleOffset},
      P{0.0f, ID::PersistRawElecAngle},
      P{0.0f, ID::PersistRawMechAngle},
      P{0.0f, ID::MechAngleOffset},
      P{3.0f, ID::CalibrateVoltage},
      P{3000, ID::CalibrateDuration},
      P{24.0f, ID::MotorSupplyVoltage},
      P{0.0f, ID::MechAnglePidP},
      P{0.0f, ID::MechAnglePidI},
      P{0.0f, ID::MechAnglePidD},
      P{0.0f, ID::MechAnglePidOutputRamp},
      P{0.0f, ID::MechAnglePidLimit},
      P{0.0f, ID::TargetPosition},
      P{0.0f, ID::TargetVelocity},
      P{0.0f, ID::VelocityPidP},
      P{0.0f, ID::VelocityPidI},
      P{0.0f, ID::VelocityPidD},
      P{0.0f, ID::VelocityPidOutputRamp},
      P{0.0f, ID::VelocityPidLimit},
      P{16, ID::VelocitySampleWindowSize},
      P{1000, ID::VelocitySampleWindowTime},
      P{10, ID::VelocitySampleMinimalDuration},
  };
  for (auto& p : properties) {
    param->add(p);
  }
}

template <typename T>
static void zephyr_backend_setup(T& injector) {
  auto os = coriander::base::LoggerStream(
      injector.template create<std::shared_ptr<coriander::base::ILogger>>());
  os << "Coriander version " << APP_VERSION_STRING << std::endl;
  os << "Reboot times: " << get_reboot_times() << std::endl;

  auto param =
      injector.template create<std::shared_ptr<coriander::ParameterBase>>();
  parameter_default_value_setup(param);

  auto diag_regsiter =
      coriander::application::zephyr::DiagnosisRegister::getInstance();
  auto diag = injector.template create<
      std::shared_ptr<coriander::application::Diagnosis>>();
  diag_regsiter->applyAll(*diag);
}

}  // namespace

int main(void) {
  auto injector =
      coriander::coriander_create_injector(zephyr_backends_bindings());

  zephyr_backend_setup(injector);

  coriander::coriander_loop(loop_hook, std::move(injector));
  return 0;
}
