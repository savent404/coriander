/**
 * @file pid.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-06
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once

#include <cstdint>

namespace coriander {
namespace motorctl {

struct Pid {
 public:
  /**
   *
   * @param P - Proportional gain
   * @param I - Integral gain
   * @param D - Derivative gain
   * @param ramp - Maximum speed of change of the output value
   * @param limit - Maximum output value
   */
  Pid(float P, float I, float D, float ramp, float limit);
  ~Pid() = default;

  float operator()(float error, float diffTime);
  void reset();

  float P;            //!< Proportional gain
  float I;            //!< Integral gain
  float D;            //!< Derivative gain
  float output_ramp;  //!< Maximum speed of change of the output value
  float limit;        //!< Maximum output value

 protected:
  float error_prev;     //!< last tracking error value
  float output_prev;    //!< last pid output value
  float integral_prev;  //!< last integral component value
};

}  // namespace motorctl
}  // namespace coriander
