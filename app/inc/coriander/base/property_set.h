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
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "coriander/base/property.h"

namespace coriander {
namespace base {

struct IPropertyVisitor {
  virtual void visit(const Property& property) noexcept = 0;
};

struct PropertyVisitorWrapper : IPropertyVisitor {
  PropertyVisitorWrapper(std::function<void(const Property&)> func)
      : func(func) {}
  virtual void visit(const Property& property) noexcept { func(property); }

 private:
  std::function<void(const Property&)> func;
};

struct IPropertySet {
  virtual ~IPropertySet() = default;

  virtual bool has(const char* name) const noexcept = 0;
  virtual bool has(const Property& property) { return has(property.name()); }

  virtual const Property& get(const char* name) const noexcept = 0;
  virtual const Property& get(const Property& property) {
    return get(property.name());
  }

  virtual void add(const Property& property) noexcept = 0;

  virtual void remove(const char* name) noexcept = 0;
  virtual void remove(const Property& property) { remove(property.name()); }

  virtual void replace(const Property& property) noexcept {
    remove(property);
    add(property);
  }
  virtual void replace(Property&& property) noexcept {
    remove(property);
    add(std::move(property));
  }

  virtual void accept(IPropertyVisitor& visitor) const = 0;
};

struct PropertyUnorderedMap : public IPropertySet {
  using map_t = std::unordered_map<std::uint32_t, Property>;

  virtual bool has(const char* name) const noexcept {
    return m_map.find(string_hash(name)) != m_map.end();
  }
  virtual bool has(std::uint32_t name_hash) const noexcept {
    return m_map.find(name_hash) != m_map.end();
  }
  virtual const Property& get(const char* name) const noexcept {
    return m_map.at(string_hash(name));
  }
  virtual const Property& get(std::uint32_t name_hash) const noexcept {
    return m_map.at(name_hash);
  }
  virtual void add(Property&& property) noexcept {
    m_map.insert({string_hash(property.name()), std::move(property)});
  }
  virtual void add(const Property& property) noexcept {
    m_map.insert({string_hash(property.name()), property});
  }
  virtual void remove(const char* name) noexcept {
    m_map.erase(string_hash(name));
  }
  virtual void remove(std::uint32_t name_hash) noexcept {
    m_map.erase(name_hash);
  }
  virtual void accept(IPropertyVisitor& visitor) const {
    for (const auto& [_, property] : m_map) {
      visitor.visit(property);
    }
  }

 private:
  map_t m_map;
};
using PropertySet = PropertyUnorderedMap;

}  // namespace base
}  // namespace coriander
