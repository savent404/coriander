/**
 * @file board_event.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-07-28
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once

#include <forward_list>
#include <memory>
#include <utility>

#include "coriander/base/ilogger.h"
#include "coriander/base/loggerstream.h"
#include "coriander/iboard_event.h"
#include "coriander/os/imutex.h"
#include "coriander/os/isemaphore.h"
#include "coriander/os/ithread.h"

namespace coriander {
struct BoardEvent : public IBoardEvent {
  using ISemaphore = coriander::os::ISemaphore;
  using IThread = coriander::os::IThread;
  using IMutex = coriander::os::IMutex;
  explicit BoardEvent(std::shared_ptr<base::ILogger> logger,
                      std::unique_ptr<ISemaphore> semaphore,
                      std::unique_ptr<IMutex> mutex,
                      std::unique_ptr<IThread> thread) noexcept
      : mLogger(logger),
        mEventCallbacks{},
        mUnlockSignal(std::move(semaphore)),
        mLock(std::move(mutex)),
        mThreadInfo(std::move(thread)) {}

  void raiseEvent(Event event) noexcept override {
    base::LoggerStream os(mLogger);

    os << "Raise event: " << static_cast<int>(event) << std::endl;

    for (auto &cb : mEventCallbacks[static_cast<int>(event)]) {
      cb(event);
    }
  }

  void registerEventCallback(Event event,
                                     EventCallback callback) noexcept override {
    mEventCallbacks[static_cast<int>(event)].push_front(callback);
  }

  void eventLock() override {
    if (mEventLocked) {
      if (mLockOwnerThread == mThreadInfo->currentThreadId()) {
        mLock->lock();
        ++mReentrantLockCount;
        mLock->unlock();
      } else {
        // wait owner thread to unlock
        mUnlockSignal->wait();
      }
    }

    mLock->lock();
    mLockOwnerThread = mThreadInfo->currentThreadId();
    mReentrantLockCount = 1;
    mEventLocked = true;
    mLock->unlock();
  }

  void eventUnlock() override {
    mLock->lock();
    --mReentrantLockCount;
    if (mReentrantLockCount == 0) {
      mEventLocked = false;
      mLockOwnerThread = 0;
      mLock->unlock();
      mUnlockSignal->post();
    } else {
      mLock->unlock();
    }
  }

 private:
  std::shared_ptr<base::ILogger> mLogger;
  std::forward_list<EventCallback>
      mEventCallbacks[static_cast<int>(Event::MAX_EVENT)];
  std::unique_ptr<ISemaphore> mUnlockSignal;
  std::unique_ptr<IMutex> mLock;
  std::unique_ptr<IThread> mThreadInfo;

  bool mEventLocked{false};
  int mReentrantLockCount{0};
  size_t mLockOwnerThread{0};
};
}  // namespace coriander
