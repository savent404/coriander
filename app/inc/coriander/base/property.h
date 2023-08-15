/**
 * @file property.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-07-30
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once

#include <cstring>
#include <functional>

#include "coriander/base/const_hash.h"
#include "coriander/base/param.h"
#include "coriander/base/type.h"
#include "param.h"

namespace coriander {
namespace base {

struct Property {
  using ParamId = coriander::base::ParamId;
  explicit Property(const Type& value, ParamId id) : m_value(value), m_id(id) {}
  explicit Property() : m_value(Invalid{}), m_id(ParamId::Unknow) {}

  Property(const Property&) = default;
  Property(Property&&) = default;

  Type& value() { return m_value; }
  const Type& value() const { return m_value; }

  void setValue(Type value) { m_value = value; }
  ParamId id() const { return m_id; }
  const char* name() const { return m_id._to_string(); }
  const char* desc() const { return ParamDescriber::getParamDescription(m_id); }

  bool operator==(const Property& other) const noexcept {
    return m_id == other.m_id &&
           TypeHelper::TypeCompare(m_value, other.m_value);
  }

 protected:
  Type m_value;
  base::ParamId m_id = base::ParamId::Unknow;
};

}  // namespace base
}  // namespace coriander