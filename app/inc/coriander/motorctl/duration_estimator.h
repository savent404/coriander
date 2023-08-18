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
  explicit DurationEstimator(std::shared_ptr<os::ISystick> systick)
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
  explicit DurationEstimator(std::shared_ptr<os::ISystick> systick)
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
  explicit DurationEstimator(std::shared_ptr<os::ISystick> systick)
      : mSystick(systick), mStart(0), mStop(0), mMax(0) {}
  void recordStart() { mStart = detail::getSystick<U>(mSystick); }
  void recordStop() {
    mStop = detail::getSystick<U>(mSystick);
    uint32_t duration = mStop - mStart;
    if (duration > mMax) {
      mMax = duration;
    }
  }
  void reset() {
    mMax = 0;
    mStart = 0;
    mStop = 0;
  }
  uint32_t getDuration() const noexcept { return mMax; }

 private:
  std::shared_ptr<os::ISystick> mSystick;
  uint32_t mStart;
  uint32_t mStop;
  uint32_t mMax;
};

template <DurationEstimatorUnit U>
struct DurationEstimator<DurationEstimatorType::Min, U> {
  explicit DurationEstimator(std::shared_ptr<os::ISystick> systick)
      : mSystick(systick), mStart(0), mStop(0), mMin(-1) {}
  void recordStart() { mStart = detail::getSystick<U>(mSystick); }
  void recordStop() {
    mStop = detail::getSystick<U>(mSystick);
    uint32_t duration = mStop - mStart;
    if (duration < mMin) {
      mMin = duration;
    }
  }
  void reset() {
    mMin = -1;
    mStart = 0;
    mStop = 0;
  }
  uint32_t getDuration() const noexcept { return mMin; }

 private:
  std::shared_ptr<os::ISystick> mSystick;
  uint32_t mStart;
  uint32_t mStop;
  uint32_t mMin;
};

template <DurationEstimatorUnit U>
struct DurationEstimator<DurationEstimatorType::Average, U> {
  explicit DurationEstimator(std::shared_ptr<os::ISystick> systick)
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
