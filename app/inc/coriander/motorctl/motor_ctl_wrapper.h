/**
 * @file motor_ctl_wrapper.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-12
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once

#include <memory>

#include "coriander/motorctl/imotorctl.h"
#include "coriander/motorctl/motor_ctl_dummy.h"
#include "coriander/motorctl/motor_ctl_position.h"
#include "coriander/motorctl/motor_ctl_velocity.h"
#include "coriander/parameter_requirements.h"
#include "coriander/parameters.h"

namespace coriander {
namespace motorctl {
struct DynamicMotorCtl : public IMotorCtl, public IParamReq {
  enum class Mode : int { Dummy = 0, Velocity, Position, MODE_MAX };

  DynamicMotorCtl(std::shared_ptr<ParameterBase> param,
                  std::shared_ptr<MotorCtlDummy> dummyMc,
                  std::shared_ptr<MotorCtlVelocity> velocityMc,
                  std::shared_ptr<MotorCtlPosition> positionMc,
                  std::shared_ptr<IParamReqValidator> paramReqValidator)
      : mParam(param),
        mMotorCtl{dummyMc, velocityMc, positionMc},
        mCurrentMc(dummyMc) {
    paramReqValidator->addParamReq(this);
  }

  void start() override {
    parseMode();
    mCurrentMc->start();
  }
  void stop() override { mCurrentMc->stop(); }
  void loop() override { mCurrentMc->loop(); }

  void emergencyStop() override { mCurrentMc->emergencyStop(); }
  bool fatalError() override { return mCurrentMc->fatalError(); }

  const ParameterRequireItem* requiredParameters() const override {
    using Type = coriander::base::TypeId;
    static const ParameterRequireItem items[] = {
        {"MotorCtl_General_Mode_RT", Type::Int32}, PARAMETER_REQ_EOF};
    return items;
  }

 protected:
  void parseMode() {
    auto mode = mParam->getValue<int>("MotorCtl_General_Mode_RT");
    if (mode < static_cast<int>(Mode::MODE_MAX)) {
      setMode(static_cast<Mode>(mode));
    } else {
      setMode(Mode::Dummy);
    }
  }

  void setMode(Mode mode) {
    if (static_cast<int>(mode) < static_cast<int>(Mode::MODE_MAX)) {
      mCurrentMc = mMotorCtl[static_cast<int>(mode)];
    }
  }

 private:
  std::shared_ptr<ParameterBase> mParam;
  std::shared_ptr<IMotorCtl> mMotorCtl[static_cast<int>(Mode::MODE_MAX)];
  std::shared_ptr<IMotorCtl> mCurrentMc;
};
}  // namespace motorctl
}  // namespace coriander
