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

#include <unordered_map>

#include "base/ilogger.h"
#include "base/loggerstream.h"
#include "parameter_requirements.h"

namespace coriander {

struct ParamReqValidator : public IParamReqValidator {
  ParamReqValidator(std::shared_ptr<ILogger> logger,
                    std::shared_ptr<ParameterBase> param)
      : mLogger(logger), mParam(param) {
    mParamMap.reserve(64);
  }

  virtual void addParamReq(IParamReq *req) {
    const ParameterRequireItem *items = req->requiredParameters();
    while (items->name != nullptr &&
           items->type != coriander::base::TypeId::Invalid) {
      mParamMap[coriander::base::string_hash(items->name)] = items;
      items++;
    }
  }

  virtual bool validate() {
    for (auto &it : mParamMap) {
      auto &item = it.second;
      if (!mParam->has(item->name)) {
        auto os = coriander::base::LoggerStream(mLogger);
        os << "required param " << item->name << " not found" << std::endl;
        return false;
      }
      auto &param = mParam->get(item->name);
      auto &v = param.value();
      if (coriander::base::TypeHelper::type(std::move(v)) != item->type) {
        auto os = coriander::base::LoggerStream(mLogger);
        os << "required param " << item->name << " type mismatch" << std::endl;
        return false;
      }
    }
    return true;
  }

 private:
  std::unordered_map<uint32_t, const ParameterRequireItem *> mParamMap;
  std::shared_ptr<ILogger> mLogger;
  std::shared_ptr<ParameterBase> mParam;
};

}  // namespace coriander