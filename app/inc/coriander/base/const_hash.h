/**
 * @file const_hash.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-07-23
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once

#include <cstdint>
#include <string>

namespace coriander {
namespace base {

namespace detail {
// FNV-1a 32bit hashing algorithm.
constexpr std::uint32_t fnv1a_32(char const* s, std::size_t count) {
  return ((count ? fnv1a_32(s, count - 1) : 2166136261u) ^ s[count]) *
         16777619u;
}

inline std::uint32_t hash_string(const char* s) {
  return fnv1a_32(s, std::char_traits<char>::length(s));
}
}  // namespace detail

constexpr std::uint32_t operator"" _hash(char const* s, std::size_t count) {
  return detail::fnv1a_32(s, count);
}

inline std::uint32_t string_hash(const char* s, std::size_t count = 0) {
  if (!count) {
    return detail::hash_string(s);
  }
  return detail::fnv1a_32(s, count);
}
}  // namespace base

}  // namespace coriander
