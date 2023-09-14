/**
 * @file parameter_requirements_validator.cc
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-09-11
 *
 * Copyright 2023 savent_gate
 *
 */
#include "coriander/parameter_requirements_validator.h"

namespace coriander {

ParamReqValidator::ParamReqValidator(std::shared_ptr<ILogger> logger,
                                     std::shared_ptr<Parameter> param)
    : mLogger(logger), mParam(param) {
  mParamMap.reserve(64);
}

void ParamReqValidator::addParamReq(IParamReq *req) {
  const ParameterRequireItem *items = req->requiredParameters();
  while (items->name != nullptr &&
         items->type != coriander::base::TypeId::Invalid) {
    mParamMap[coriander::base::string_hash(items->name)] = items;
    items++;
  }
}

bool ParamReqValidator::validate() {
  for (auto &it : mParamMap) {
    auto &item = it.second;
    if (!mParam->has(item->name)) {
      CORIANDER_LOG_WARN(mLogger, "required param {} not found", item->name);
      return false;
    }
    auto &param = mParam->get(item->name);
    auto &v = param.value();
    if (coriander::base::TypeHelper::type(std::move(v)) != item->type) {
      CORIANDER_LOG_WARN(mLogger, "required param {} type mismatch",
                         item->name);
      return false;
    }
  }
  return true;
}

}  // namespace coriander
