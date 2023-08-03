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
#include <span>

#include "coriander/base/property_set.h"
#include "coriander/base/type.h"

namespace coriander {

using namespace coriander::base;

struct ParameterBase : public base::PropertySet {
  template <typename T>
  T getValue(const char* name) const noexcept {
    return get<T>(base::PropertySet::get(name).value());
  }
  template <typename T>
  T getValue(uint32_t name_hash) const noexcept {
    return get<T>(base::PropertySet::get(name_hash).value());
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

  /**
   * @brief Set the Value object
   *
   * @param name_hash
   * @param t
   * @note Can't create new property if not exist
   * @return true set value success
   * @return false set value failed
   */
  bool setValue(uint32_t name_hash, Type t) noexcept {
    if (has(name_hash)) {
      auto p = get(name_hash);
      p.setValue(t);
      remove(name_hash);
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
}  // namespace coriander
