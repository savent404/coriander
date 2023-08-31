/**
 * @file posix_persistent_parameter.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-31
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once

#include <stdio.h>

#include <memory>

#include "coriander/parameters.h"

namespace coriander {
namespace posix {
struct PersistentParameter : public IPersistentParameter {
  constexpr const char *get_filepath() { return "/tmp/parameter.bin"; }
  virtual bool save() {
    ParameterMemoryMapper mapper;

    auto map = mapper.map(this);
    if (map.empty()) {
      return false;
    }

    auto fp = fopen(get_filepath(), "wb+");
    if (fp == nullptr) {
      return false;
    }

    auto res = fwrite(map.data(), map.size(), 1, fp);
    if (res != 1) {
      fclose(fp);
      return false;
    }
    fclose(fp);
    mapper.unmap();
    return true;
  }
  virtual bool load() {
    ParameterMemoryMapper mapper;

    auto fp = fopen(get_filepath(), "rb");
    if (fp == nullptr) {
      return false;
    }

    fseek(fp, 0, SEEK_END);
    auto size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    auto buf = std::make_unique<uint8_t[]>(size);
    auto res = fread(buf.get(), static_cast<size_t>(size), 1, fp);
    if (res != 1) {
      fclose(fp);
      return false;
    }
    fclose(fp);

    auto map = std::span<uint8_t>{buf.get(), static_cast<size_t>(size)};
    res = mapper.recovery(map, this);
    if (!res) {
      return false;
    }

    return true;
  }
};
}  // namespace posix
}  // namespace coriander
