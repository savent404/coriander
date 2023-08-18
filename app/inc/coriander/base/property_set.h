/**
 * @file propert_set.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-07-23
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once

#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>

#include "coriander/base/property.h"
#include "coriander/base/type.h"

namespace coriander {
namespace base {

struct IPropertyVisitor {
  virtual void visit(const Property& property) noexcept = 0;
};

struct PropertyVisitorWrapper : IPropertyVisitor {
  explicit PropertyVisitorWrapper(std::function<void(const Property&)> func)
      : func(func) {}
  virtual void visit(const Property& property) noexcept { func(property); }

 private:
  std::function<void(const Property&)> func;
};

struct PropertyStaticMap {
  using map_t = std::array<std::unique_ptr<Property>, ParamId::MAX_PARAM_ID>;

  bool has(const char* name) const noexcept {
    auto t = ParamId::_from_string_nothrow(name);
    if (t) {
      return has(*t);
    }
    return false;
  }

  bool has(ParamId id) const noexcept {
    return m_map[id._to_index()] != nullptr;
  }

  bool has(const Property& property) const noexcept {
    return has(property.name());
  }

  const Property& get(const char* name) const noexcept {
    static const Property empty;
    auto t = ParamId::_from_string_nothrow(name);
    if (t) {
      return get(*t);
    }
    return empty;
  }

  const Property& get(ParamId id) const noexcept {
    return *m_map[id._to_index()];
  }

  virtual const Property& get(const Property& property) const noexcept {
    return get(property.name());
  }

  void add(Property&& property) noexcept {
    m_map[property.id()._to_index()] =
        std::make_unique<Property>(std::move(property));
  }

  void add(const Property& property) noexcept {
    m_map[property.id()._to_index()] = std::make_unique<Property>(property);
  }

  void remove(const char* name) noexcept {
    auto t = ParamId::_from_string_nothrow(name);
    if (t) {
      remove(*t);
    }
  }
  void remove(ParamId id) noexcept { m_map[id._to_index()] = nullptr; }

  virtual void remove(const Property& property) noexcept {
    remove(property.name());
  }

  void replace(const Property& property) noexcept {
    m_map[property.id()._to_index()] = std::make_unique<Property>(property);
  }

  void replace(Property&& property) noexcept {
    m_map[property.id()._to_index()] = std::make_unique<Property>(property);
  }

  void accept(IPropertyVisitor* visitor) const {
    for (auto& p : m_map) {
      if (p) {
        visitor->visit(*p);
      }
    }
  }

 private:
  map_t m_map;
};
using PropertySet = PropertyStaticMap;

}  // namespace base
}  // namespace coriander
