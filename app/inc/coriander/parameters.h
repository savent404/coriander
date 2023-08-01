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

#include <span>
#include <memory>

#include "coriander/base/property_set.h"

namespace coriander {

using ParameterBase = base::PropertySet;

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
