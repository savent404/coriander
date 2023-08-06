/**
 * @file duration_estimator.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-06
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once

#include <memory>

#include "coriander/os/isystick.h"

namespace coriander {
namespace motorctl {

namespace detail {
enum class DurationEstimatorType {
  OneShot,
  Average,
  Max,
  Min,
};

enum class DurationEstimatorUnit {
  MS,
  US,
};

template <DurationEstimatorUnit U>
inline uint32_t getSystick(std::shared_ptr<os::ISystick> systick);

template <>
inline uint32_t getSystick<DurationEstimatorUnit::MS>(
    std::shared_ptr<os::ISystick> systick) {
  return systick->systick_ms();
}

template <>
inline uint32_t getSystick<DurationEstimatorUnit::US>(
    std::shared_ptr<os::ISystick> systick) {
  return systick->systick_us();
}

template <DurationEstimatorType T, DurationEstimatorUnit U>
struct DurationEstimator {
  DurationEstimator(std::shared_ptr<os::ISystick> systick)
      : mSystick(systick) {}
  void recordStart();
  void recordStop();
  void reset();
  uint32_t getDuration() const noexcept;

 private:
  std::shared_ptr<os::ISystick> mSystick;
  uint32_t getSystick();
};

template <DurationEstimatorUnit U>
struct DurationEstimator<DurationEstimatorType::OneShot, U> {
  DurationEstimator(std::shared_ptr<os::ISystick> systick)
      : mSystick(systick), mStart(0), mStop(0) {}

  void recordStart() { mStart = detail::getSystick<U>(mSystick); }
  void recordStop() { mStop = detail::getSystick<U>(mSystick); }
  void reset() {
    mStart = 0;
    mStop = 0;
  }
  uint32_t getDuration() const noexcept { return mStop - mStart; }

 private:
  std::shared_ptr<os::ISystick> mSystick;
  uint32_t mStart;
  uint32_t mStop;
};

template <DurationEstimatorUnit U>
struct DurationEstimator<DurationEstimatorType::Max, U> {
  DurationEstimator(std::shared_ptr<os::ISystick> systick)
      : mSystick(systick), mStart(0), mStop(0), max(0) {}
  void recordStart() { mStart = detail::getSystick<U>(mSystick); }
  void recordStop() {
    mStop = detail::getSystick<U>(mSystick);
    uint32_t duration = mStop - mStart;
    if (duration > max) {
      max = duration;
    }
  }
  void reset() {
    max = 0;
    mStart = 0;
    mStop = 0;
  }
  uint32_t getDuration() const noexcept { return max; }

 private:
  std::shared_ptr<os::ISystick> mSystick;
  uint32_t mStart;
  uint32_t mStop;
  uint32_t max;
};

template <DurationEstimatorUnit U>
struct DurationEstimator<DurationEstimatorType::Min, U> {
  DurationEstimator(std::shared_ptr<os::ISystick> systick)
      : mSystick(systick), mStart(0), mStop(0), min(-1) {}
  void recordStart() { mStart = detail::getSystick<U>(mSystick); }
  void recordStop() {
    mStop = detail::getSystick<U>(mSystick);
    uint32_t duration = mStop - mStart;
    if (duration < min) {
      min = duration;
    }
  }
  void reset() {
    min = -1;
    mStart = 0;
    mStop = 0;
  }
  uint32_t getDuration() const noexcept { return min; }

 private:
  std::shared_ptr<os::ISystick> mSystick;
  uint32_t mStart;
  uint32_t mStop;
  uint32_t min;
};

template <DurationEstimatorUnit U>
struct DurationEstimator<DurationEstimatorType::Average, U> {
  DurationEstimator(std::shared_ptr<os::ISystick> systick)
      : mSystick(systick), mStart(0), mStop(0), sum(0), count(0) {}

  void recordStart() { mStart = detail::getSystick<U>(mSystick); }
  void recordStop() {
    mStop = detail::getSystick<U>(mSystick);
    sum += mStop - mStart;
    count++;
  }

  void reset() {
    sum = 0;
    count = 0;
    mStart = 0;
    mStop = 0;
  }

  uint32_t getDuration() const noexcept { return sum / count; }

 private:
  std::shared_ptr<os::ISystick> mSystick;
  uint32_t mStart;
  uint32_t mStop;
  uint64_t sum;
  uint32_t count;
};
}  // namespace detail

using detail::DurationEstimator;
using detail::DurationEstimatorType;
using detail::DurationEstimatorUnit;

using DurationEstimatorDefault =
    DurationEstimator<DurationEstimatorType::OneShot,
                      DurationEstimatorUnit::MS>;

}  // namespace motorctl
}  // namespace coriander
