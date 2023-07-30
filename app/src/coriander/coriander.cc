/**
 * @file coriander.cc
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-07-30
 *
 * Copyright 2023 savent_gate
 *
 */
#include "coriander/coriander.h"

#include <boost/di.hpp>
#include <memory>

// interfaces
#include "coriander/application/iappstatus.h"
#include "coriander/iboard_event.h"
#include "coriander/iboard_state.h"

// implements
#include "coriander/application/appstatus.h"
#include "coriander/board_event.h"
#include "coriander/board_state.h"
#include "coriander/board_state_calibrate_handler.h"
#include "coriander/board_state_error_handler.h"
#include "coriander/board_state_firmware_update_handler.h"
#include "coriander/board_state_init_handler.h"
#include "coriander/board_state_run_handler.h"
#include "coriander/board_state_standby_handler.h"

namespace {

static auto createInjector() {
  static auto mInjector = boost::di::make_injector(
      boost::di::bind<coriander::IAppStatus>.to<coriander::AppStatus>(),
      boost::di::bind<coriander::IBoardState>.to<coriander::BoardState>(),
      boost::di::bind<coriander::IBoardStateInitHandler>.to<coriander::BoardStateInitHandler>(),
      boost::di::bind<coriander::IBoardStateStandbyHandler>.to<coriander::BoardStateStandbyHandler>(),
      boost::di::bind<coriander::IBoardStateRunHandler>.to<coriander::BoardStateRunHandler>(),
      boost::di::bind<coriander::IBoardStateErrorHandler>.to<coriander::BoardStateErrorHandler>(),
      boost::di::bind<coriander::IBoardStateCalibrateHandler>.to<coriander::BoardStateCalibrateHandler>(),
      boost::di::bind<coriander::IBoardStateFirmwareUpdateHandler>.to<coriander::BoardStateFirmwareUpdateHandler>(),
      boost::di::bind<coriander::IBoardEvent>.to<coriander::BoardEvent>());
  return &mInjector;
}

struct CorianderInstance {
  CorianderInstance()
      : mBoard(createInjector()
                   ->create<std::shared_ptr<coriander::IBoardState>>()) {}

  void run() { mBoard->loop(); }
  std::shared_ptr<coriander::IBoardState> mBoard;
};

}  // namespace

extern "C" {
coriander_instance_t create_coriander_instance(void) {
  auto t = new CorianderInstance();
  return reinterpret_cast<void *>(t);
}

void coriander_run(coriander_instance_t instance) {
  auto coriander = reinterpret_cast<CorianderInstance *>(instance);
  coriander->run();
}

void destroy_coriander_instance(coriander_instance_t instance) {
  auto coriander = reinterpret_cast<CorianderInstance *>(instance);
  delete coriander;
}
}  // extern "C"
