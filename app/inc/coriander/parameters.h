/**
 * @file parameters.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-01
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

#include <memory>
#include <span>  // NOLINT

#include "coriander/base/property_set.h"
#include "coriander/base/type.h"

namespace coriander {

using coriander::base::operator""_hash;
using coriander::base::ParamId;
using coriander::base::Type;
using coriander::base::TypeId;

struct ParameterBase : public base::PropertySet {
  template <typename T>
  T getValue(const char* name) const noexcept {
    return std::get<T>(base::PropertySet::get(name).value());
  }
  template <typename T>
  T getValue(ParamId id) const noexcept {
    return std::get<T>(base::PropertySet::get(id).value());
  }

  /**
   * @brief Set the Value object
   *
   * @note Can't create new property if not exist
   * @param name
   * @param t
   */
  bool setValue(const char* name, Type t) noexcept {
    if (has(name)) {
      auto p = get(name);
      p.setValue(t);
      remove(name);
      add(p);
      return true;
    } else {
      return false;
    }
  }

  bool setValue(ParamId id, Type t) noexcept {
    if (has(id)) {
      auto p = get(id);
      p.setValue(t);
      remove(id);
      add(p);
      return true;
    } else {
      return false;
    }
  }
};

struct ParameterMemoryMapper {
  using Property = base::Property;

  std::span<uint8_t> map(const ParameterBase* param) noexcept;
  void unmap() noexcept;

  bool isValid(std::span<uint8_t> data) const noexcept;
  bool recovery(std::span<uint8_t> data, ParameterBase* param) const noexcept;

 private:
  std::unique_ptr<uint8_t[]> mMapped = nullptr;
  size_t mMappedSize = 0;
};

struct IPersistentParameter : public ParameterBase {
  virtual bool save() = 0;
  virtual bool load() = 0;
};

using Parameter = ParameterBase;
}  // namespace coriander
