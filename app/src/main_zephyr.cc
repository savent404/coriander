/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#include <app_version.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/sem.h>

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

K_SEM_DEFINE(main_wakeup, 0, 1);
static void main_tick_expired(struct k_timer* timer) {
  k_sem_give(&main_wakeup);
}
K_TIMER_DEFINE(main_tick, main_tick_expired, NULL);

// provided by board specific code
extern void board_parameter_default_value_setup(
    std::shared_ptr<coriander::Parameter> param);

namespace {

static void loop_hook() {
  // wait for main loop tick, max wait time is 1ms
  k_sem_take(&main_wakeup, K_MSEC(1));
}

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


template <typename T>
static void zephyr_backend_setup(T* injector) {
  auto os = coriander::base::LoggerStream(
      injector->template create<std::shared_ptr<coriander::base::ILogger>>());
  os << "Coriander version " << APP_VERSION_STRING << std::endl;
  os << "Reboot times: " << get_reboot_times() << std::endl;

  auto param =
      injector->template create<std::shared_ptr<coriander::Parameter>>();
  board_parameter_default_value_setup(param);

  auto diag_regsiter =
      coriander::application::zephyr::DiagnosisRegister::getInstance();
  auto diag = injector->template create<
      std::shared_ptr<coriander::application::Diagnosis>>();
  diag_regsiter->applyAll(diag.get());

  // bring up coriander main loop's tick, frequency is 2KHz
  k_timer_start(&main_tick, K_USEC(500), K_USEC(500));
}

}  // namespace

int main(void) {
  auto injector =
      coriander::coriander_create_injector(zephyr_backends_bindings());

  zephyr_backend_setup(&injector);

  coriander::coriander_loop(loop_hook, std::move(injector));
  return 0;
}
