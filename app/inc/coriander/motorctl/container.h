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

#include "coriander/motorctl/encoder_elec_angle_estimator.h"
#include "coriander/motorctl/encoder_mech_angle_estimator.h"
#include "coriander/motorctl/motor_ctl_calibrate.h"
#include "coriander/motorctl/velocity_estimator.h"

namespace coriander {
namespace motorctl {
namespace detail {
static inline auto createInjector() {
  using boost::di::bind;
  using boost::di::make_injector;
  return make_injector(
      bind<IElecAngleEstimator>().to<EncoderElecAngleEstimator>(),
      bind<IMechAngleEstimator>().to<EncoderMechAngleEstimator>(),
      bind<IVelocityEstimator>().to<VelocityEstimator>(),
      bind<MotorCtlCalibrate>().to<MotorCtlCalibrate>());
}

}  // namespace detail
}  // namespace motorctl
}  // namespace coriander
