/**
 * @file parameter_requirements_validator.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-14
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once

#include <memory>
#include <unordered_map>
#include <utility>

#include "base/ilogger.h"
#include "coriander/parameter_requirements.h"

namespace coriander {

struct ParamReqValidator : public IParamReqValidator {
  using ILogger = coriander::base::ILogger;
  using Parameter = coriander::Parameter;
  using ParameterRequireItem = coriander::ParameterRequireItem;

  ParamReqValidator(std::shared_ptr<ILogger> logger,
                    std::shared_ptr<Parameter> param);

  /**
   * @brief Add a requirement to the validator
   *
   * @param req see @c IParamReq
   */
  virtual void addParamReq(IParamReq *req);

  /**
   * @brief validate the parameter requirements is satisfied
   *
   * @return true validate success
   * @return false validate failed
   */
  virtual bool validate();

 private:
  std::unordered_map<uint32_t, const ParameterRequireItem *> mParamMap;
  std::shared_ptr<ILogger> mLogger;
  std::shared_ptr<Parameter> mParam;
};

}  // namespace coriander
