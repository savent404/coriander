/**
 * @file container.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-05
 *
 * Copyright 2023 savent_gate
 *
 */
#include <boost/di.hpp>

#include "coriander/base/const_hash.h"
#include "coriander/motorctl/encoder_elec_angle_estimator.h"
#include "coriander/motorctl/encoder_mech_angle_estimator.h"
#include "coriander/motorctl/imotorctl.h"
#include "coriander/motorctl/ivelocity_estimator.h"
#include "coriander/motorctl/motor_ctl_calibrate.h"
#include "coriander/motorctl/motor_ctl_dummy.h"
#include "coriander/motorctl/motor_ctl_velocity.h"
#include "coriander/motorctl/velocity_estimator.h"
#include "coriander/parameters.h"

namespace coriander {
namespace motorctl {
namespace detail {
static inline auto createInjector() {
  using boost::di::bind;
  using boost::di::make_injector;
  using coriander::base::operator""_hash;
  return make_injector(
      bind<IElecAngleEstimator>().to<EncoderElecAngleEstimator>(),
      bind<IMechAngleEstimator>().to<EncoderMechAngleEstimator>(),
      bind<IVelocityEstimator>().to<VelocityEstimator>(),
      bind<MotorCtlCalibrate>().to<MotorCtlCalibrate>(),
      bind<IMotorCtl>().to([](const auto &injector)
                               -> std::shared_ptr<IMotorCtl> {
        int mode = 0;
        auto param = injector.template create<std::shared_ptr<ParameterBase>>();
        if (param->has("motorctl.mode"_hash)) {
          mode = param->template getValue<int>("motorctl.mode"_hash);
        }
        switch (mode) {
          case 0:
          default:
            return injector.template create<std::shared_ptr<MotorCtlDummy>>();
          case 1:
            return injector
                .template create<std::shared_ptr<MotorCtlVelocity>>();
        }
      }));
}

}  // namespace detail
}  // namespace motorctl
}  // namespace coriander
