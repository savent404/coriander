/**
 * @file ut_board_state.cc
 * @author Z004TPFF (yuankai.liao@siemens.com)
 * @brief
 * @date 2023-07-31
 *
 * Copyright 2023 savent_gate
 *
 */
#include <gtest/gtest.h>

#include <boost/di.hpp>
#include <functional>

#include "coriander/board_event.h"
#include "coriander/board_state.h"
#include "posix_logger.h"

namespace coriander {
struct custom_hook {
  std::function<void()> custom_enter;
  std::function<void()> custom_loop;
  std::function<void()> custom_exit;
};

struct dummyInit : public IBoardStateInitHandler, public custom_hook {
  virtual void onEnter() noexcept override {
    if (custom_enter) {
      custom_enter();
    }
  }
  virtual void onExit() noexcept override {
    if (custom_exit) {
      custom_exit();
    }
  }
  virtual void onLoop() noexcept override {
    if (custom_loop) {
      custom_loop();
    }
  }
};

struct dummyStandby : public IBoardStateStandbyHandler, public custom_hook {
  virtual void onEnter() noexcept override {
    if (custom_enter) {
      custom_enter();
    }
  }
  virtual void onExit() noexcept override {
    if (custom_exit) {
      custom_exit();
    }
  }
  virtual void onLoop() noexcept override {
    if (custom_loop) {
      custom_loop();
    }
  }
};

struct dummyRun : public IBoardStateRunHandler, public custom_hook {
  virtual void onEnter() noexcept override {
    if (custom_enter) {
      custom_enter();
    }
  }
  virtual void onExit() noexcept override {
    if (custom_exit) {
      custom_exit();
    }
  }
  virtual void onLoop() noexcept override {
    if (custom_loop) {
      custom_loop();
    }
  }
};

struct dummyError : public IBoardStateErrorHandler, public custom_hook {
  virtual void onEnter() noexcept override {
    if (custom_enter) {
      custom_enter();
    }
  }
  virtual void onExit() noexcept override {
    if (custom_exit) {
      custom_exit();
    }
  }
  virtual void onLoop() noexcept override {
    if (custom_loop) {
      custom_loop();
    }
  }
};

struct dummyCalibrate : public IBoardStateCalibrateHandler, public custom_hook {
  virtual void onEnter() noexcept override {
    if (custom_enter) {
      custom_enter();
    }
  }
  virtual void onExit() noexcept override {
    if (custom_exit) {
      custom_exit();
    }
  }
  virtual void onLoop() noexcept override {
    if (custom_loop) {
      custom_loop();
    }
  }
};

struct dummyFirmwareUpdate : public IBoardStateFirmwareUpdateHandler,
                             public custom_hook {
  virtual void onEnter() noexcept override {
    if (custom_enter) {
      custom_enter();
    }
  }
  virtual void onExit() noexcept override {
    if (custom_exit) {
      custom_exit();
    }
  }
  virtual void onLoop() noexcept override {
    if (custom_loop) {
      custom_loop();
    }
  }
};

struct dummyReboot : public IBoardStateRebootHandler, public custom_hook {
  virtual void onEnter() noexcept override {
    if (custom_enter) {
      custom_enter();
    }
  }
  virtual void onExit() noexcept override {
    if (custom_exit) {
      custom_exit();
    }
  }
  virtual void onLoop() noexcept override {
    if (custom_loop) {
      custom_loop();
    }
  }
};

}  // namespace coriander

TEST(BoardState, basic) {
  auto injector = boost::di::make_injector(
      boost::di::bind<coriander::base::ILogger>.to<coriander::base::posix::Logger>(),
      boost::di::bind<coriander::IBoardEvent>.to<coriander::BoardEvent>(),
      boost::di::bind<coriander::IBoardStateInitHandler>.to<coriander::dummyInit>(),
      boost::di::bind<coriander::IBoardStateStandbyHandler>.to<coriander::dummyStandby>(),
      boost::di::bind<coriander::IBoardStateRunHandler>.to<coriander::dummyRun>(),
      boost::di::bind<coriander::IBoardStateErrorHandler>.to<coriander::dummyError>(),
      boost::di::bind<coriander::IBoardStateCalibrateHandler>.to<coriander::dummyCalibrate>(),
      boost::di::bind<coriander::IBoardStateFirmwareUpdateHandler>.to<coriander::dummyFirmwareUpdate>(),
      boost::di::bind<coriander::IBoardStateRebootHandler>.to<coriander::dummyReboot>());

  auto event = injector.create<std::shared_ptr<coriander::BoardEvent> >();
  auto initHandler = injector.create<std::shared_ptr<coriander::dummyInit> >();
  auto standbyHandler =
      injector.create<std::shared_ptr<coriander::dummyStandby> >();
  auto runHandler = injector.create<std::shared_ptr<coriander::dummyRun> >();
  auto errorHandler =
      injector.create<std::shared_ptr<coriander::dummyError> >();
  auto calibrateHandler =
      injector.create<std::shared_ptr<coriander::dummyCalibrate> >();
  auto firmwareUpdateHandler =
      injector.create<std::shared_ptr<coriander::dummyFirmwareUpdate> >();
  auto rebootHandler =
      injector.create<std::shared_ptr<coriander::dummyReboot> >();
  // board call init->entry at constructor
  initHandler->custom_enter = [event]() {
    event->raiseEvent(coriander::IBoardEvent::Event::BoardInited);
  };

  coriander::BoardState board(initHandler, standbyHandler, runHandler,
                              errorHandler, calibrateHandler,
                              firmwareUpdateHandler, rebootHandler, event);
  ASSERT_EQ(board.getState(), coriander::BoardState::State::Standby);

  // Test MotorStart, MotorStop
  standbyHandler->custom_loop = [event]() {
    event->raiseEvent(coriander::IBoardEvent::Event::MotorStart);
  };
  runHandler->custom_loop = [event]() {
    event->raiseEvent(coriander::IBoardEvent::Event::MotorStop);
  };
  board.loop();
  ASSERT_EQ(board.getState(), coriander::BoardState::State::Run);
  board.loop();
  ASSERT_EQ(board.getState(), coriander::BoardState::State::Standby);
  standbyHandler->custom_loop = nullptr;
  runHandler->custom_loop = nullptr;

  // Test MotorStart->Crap->CrapHandled
  standbyHandler->custom_loop = [event]() {
    event->raiseEvent(coriander::IBoardEvent::Event::MotorStart);
  };
  runHandler->custom_loop = [event]() {
    event->raiseEvent(coriander::IBoardEvent::Event::Crap);
  };
  errorHandler->custom_loop = [event]() {
    event->raiseEvent(coriander::IBoardEvent::Event::CrapHandled);
  };
  board.loop();
  ASSERT_EQ(board.getState(), coriander::BoardState::State::Run);
  board.loop();
  ASSERT_EQ(board.getState(), coriander::BoardState::State::Error);
  board.loop();
  ASSERT_EQ(board.getState(), coriander::BoardState::State::Standby);
  standbyHandler->custom_loop = nullptr;
  runHandler->custom_loop = nullptr;
  errorHandler->custom_loop = nullptr;

  // Test MotorStart->Crap->CrapUnHandled
  standbyHandler->custom_loop = [event]() {
    event->raiseEvent(coriander::IBoardEvent::Event::MotorStart);
  };
  runHandler->custom_loop = [event]() {
    event->raiseEvent(coriander::IBoardEvent::Event::Crap);
  };
  errorHandler->custom_loop = [event]() {
    event->raiseEvent(coriander::IBoardEvent::Event::CrapUnhandle);
  };
  board.loop();
  ASSERT_EQ(board.getState(), coriander::BoardState::State::Run);
  board.loop();
  ASSERT_EQ(board.getState(), coriander::BoardState::State::Error);
  board.loop();
  ASSERT_EQ(board.getState(), coriander::BoardState::State::Reboot);
  standbyHandler->custom_loop = nullptr;
  runHandler->custom_loop = nullptr;
  errorHandler->custom_loop = nullptr;
  board.setState(coriander::BoardState::State::Standby);

  // Test FirmwareUpdate
  standbyHandler->custom_loop = [event]() {
    event->raiseEvent(coriander::IBoardEvent::Event::FirmwareStartUpdate);
  };
  firmwareUpdateHandler->custom_loop = [event]() {
    event->raiseEvent(coriander::IBoardEvent::Event::FirmwareUpdateDone);
  };
  board.loop();
  ASSERT_EQ(board.getState(), coriander::BoardState::State::FirmwareUpdate);
  board.loop();
  ASSERT_EQ(board.getState(), coriander::BoardState::State::Standby);
  standbyHandler->custom_loop = nullptr;
  firmwareUpdateHandler->custom_loop = nullptr;

  // Test FirmwareUpdate->Crap->CrapHandled
  standbyHandler->custom_loop = [event]() {
    event->raiseEvent(coriander::IBoardEvent::Event::FirmwareStartUpdate);
  };
  firmwareUpdateHandler->custom_loop = [event]() {
    event->raiseEvent(coriander::IBoardEvent::Event::Crap);
  };
  errorHandler->custom_loop = [event]() {
    event->raiseEvent(coriander::IBoardEvent::Event::CrapHandled);
  };
  board.loop();
  ASSERT_EQ(board.getState(), coriander::BoardState::State::FirmwareUpdate);
  board.loop();
  ASSERT_EQ(board.getState(), coriander::BoardState::State::Error);
  board.loop();
  ASSERT_EQ(board.getState(), coriander::BoardState::State::Standby);
  standbyHandler->custom_loop = nullptr;
  firmwareUpdateHandler->custom_loop = nullptr;
  errorHandler->custom_loop = nullptr;

  // Test Calibrate
  standbyHandler->custom_loop = [event]() {
    event->raiseEvent(coriander::IBoardEvent::Event::CalibrateStart);
  };
  calibrateHandler->custom_loop = [event]() {
    event->raiseEvent(coriander::IBoardEvent::Event::CalibrateDone);
  };
  board.loop();
  ASSERT_EQ(board.getState(), coriander::BoardState::State::Calibrate);
  board.loop();
  ASSERT_EQ(board.getState(), coriander::BoardState::State::Standby);
  standbyHandler->custom_loop = nullptr;
  calibrateHandler->custom_loop = nullptr;
}
