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

#include "coriander/container.h"

namespace {

static auto createInjector() {
  static auto mInjector = coriander::detail::createInjector();
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
