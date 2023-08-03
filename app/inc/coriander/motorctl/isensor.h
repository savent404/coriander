/**
 * @file isensor.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-03
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

namespace coriander {
namespace motorctl {

/**
 * @brief interface for sensor
 *
 * @note put init routine into enable(), when disable()/enable() is called, the
 * sensor has changes to update internal vars just like parameters outside.
 */
struct ISensor {
  virtual ~ISensor() = default;

  virtual void enable() = 0;
  virtual void disable() = 0;
  virtual bool enabled() = 0;
  virtual void sync() = 0;

  virtual bool needSync(unsigned syncId) {
    if (this->mSyncId < syncId) {
      this->mSyncId = syncId;
      return true;
    }
    return false;
  }

  /**
   * @brief set the sensor to zero, calibrate offset
   */
  virtual void calibrate() = 0;
  virtual bool needCalibrate() = 0;

 protected:
  unsigned mSyncId = 0;
};

}  // namespace motorctl
}  // namespace coriander
