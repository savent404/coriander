/**
 * @file parameter_requirements.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-13
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once

#include "coriander/base/const_hash.h"
#include "coriander/parameters.h"

namespace coriander {

struct ParameterRequireItem {
  const char* name;
  uint32_t name_hash;
  base::TypeId type;
};

struct IParamReq {
  virtual ~IParamReq() = default;
  virtual const ParameterRequireItem* requiredParameters() const = 0;
};

struct IParamReqValidator {
  virtual ~IParamReqValidator() = default;
  virtual void addParamReq(IParamReq* req) = 0;
  virtual bool validate() = 0;
};

}  // namespace coriander
