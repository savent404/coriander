/**
 * @file imotorctl.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-08-03
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

namespace coriander {
namespace motorctl {

struct IMotorCtl {
  virtual ~IMotorCtl() = default;
  virtual void start() = 0;
  virtual void stop() = 0;
  virtual void loop() = 0;

  virtual void emergencyStop() = 0;
  virtual bool fatalError() = 0;
};

}  // namespace motorctl
}  // namespace coriander
