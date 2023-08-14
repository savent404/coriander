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
  using P = Property;

  const static Property properties[] = {
      P{0, "motorctl.mode", "0:dummy, 1:velocity, 2:position"},
      P{1, "pole_pair", "motor pole pair"},
      P{0.0f, "elec_angle_offset", "electrical angle offset, unit: degree"},
      P{0.0f, "persist_raw_elec_angle",
        "electrical angle, persistent value, unit: degree"},
      P{0.0f, "persist_raw_mech_angle",
        "mechanical angle, persistent value. unit: degree"},
      P{0.0f, "mech_angle_offset", "mechanical angle offset, unit: degree"},
      P{3.0f, "calibrate_voltage", "calibrate voltage, unit: volt"},
      P{3000, "calibrate_duration", "calibrate duration, unit: ms"},
      P{24.0f, "motor_supply_voltage", "motor supply voltage, unit: volt"},
      P{0.0f, "mech_angle_pid_p"},
      P{0.0f, "mech_angle_pid_i"},
      P{0.0f, "mech_angle_pid_d"},
      P{0.0f, "mech_angle_pid_output_ramp", "maxium ramp of output"},
      P{0.0f, "mech_angle_pid_limit", "maxium output"},
      P{0.0f, "target_position", "target position, unit: degree"},
      P{0.0f, "target_velocity", "target velocity, unit: RPM"},
      P{0.0f, "velocity_pid_p"},
      P{0.0f, "velocity_pid_i"},
      P{0.0f, "velocity_pid_d"},
      P{0.0f, "velocity_pid_output_ramp", "maxium ramp of output"},
      P{0.0f, "velocity_pid_limit", "maxium output"},
      P{16, "velocity_sample_window_size",
        "window size of velocity estimator, default: 16"},
      P{1000, "velocity_sample_window_time",
        "max window time of velocity estimator, default: 1000ms"},
      P{10, "velocity_sample_minimal_duration",
        "minimal duration of velocity estimator, default: 10ms"},
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
