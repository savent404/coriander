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
#include "coriander/base/type.h"

namespace coriander {
namespace base {

struct Property {
  explicit Property(const Type& value, const char* name,
                    const char* desc = "null")
      : m_value(value), m_name(name), m_desc(desc) {}
  explicit Property(Type&& value, const char* name, const char* desc = "null")
      : m_value(std::move(value)), m_name(name), m_desc(desc) {}

  Property(const Property&) = default;
  Property(Property&&) = default;

  Type& value() { return m_value; }
  const Type& value() const { return m_value; }
  void setValue(Type value) { m_value = value; }
  const char* name() const { return m_name; }
  const char* desc() const { return m_desc; }

  bool operator==(const Property& other) const noexcept {
    return !std::strcmp(m_name, other.m_name) &&
           !std::strcmp(m_desc, other.m_desc) &&
           TypeHelper::TypeCompare(m_value, other.m_value);
  }

 protected:
  Type m_value;
  const char* m_name;
  const char* m_desc;
};

}  // namespace base
}  // namespace coriander