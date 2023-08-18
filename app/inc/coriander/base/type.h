/**
 * @file type.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-07-30
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once

#include <cstddef>
#include <cstdint>
#include <tuple>
#include <variant>
#include <cstring>

namespace coriander {
namespace base {

using std::get;
using std::make_tuple;
using std::tie;
using std::tuple;
using std::variant;

template <typename T>
using TripleBaseType = tuple<T, T, T>;

struct Invalid {};

using Type = variant<std::int32_t, std::int64_t, std::uint32_t, std::uint64_t,
                     float, double, const char*, TripleBaseType<std::int32_t>,
                     TripleBaseType<float>, TripleBaseType<double>, Invalid>;

enum class TypeId : std::uint8_t {
  Int32 = 0,
  Int64,
  UInt32,
  UInt64,
  Float,
  Double,
  String,
  TripleInt32,
  TripleFloat,
  TripleDouble,
  MAX_TYPE,
  Invalid = MAX_TYPE
};

struct TypeHelper {
  constexpr static const char* name(TypeId id) noexcept {
    return name_[static_cast<std::size_t>(id)];
  }
  constexpr static TypeId type(std::uint8_t id) noexcept {
    return static_cast<TypeId>(id);
  }
  static TypeId type(const char* name) noexcept {
    for (std::size_t i = 0; i < sizeof(name_) / sizeof(name_[0]); ++i) {
      if (std::strcmp(name, name_[i]) == 0) {
        return static_cast<TypeId>(i);
      }
    }
    return TypeId::Invalid;
  }
  constexpr static TypeId type(Type&& t) noexcept {
    return static_cast<TypeId>(t.index());
  }
  constexpr static TypeId type(const Type& t) noexcept {
    return static_cast<TypeId>(t.index());
  }
  static bool TypeCompare(const Type& rhs, const Type& lhs) noexcept {
    bool result = false;
    if (rhs.index() != lhs.index()) {
      return false;
    }

    switch (TypeHelper::type(rhs)) {
      case TypeId::Int32:
        result = get<std::int32_t>(rhs) == get<std::int32_t>(lhs);
        break;
      case TypeId::Int64:
        result = get<std::int64_t>(rhs) == get<std::int64_t>(lhs);
        break;
      case TypeId::UInt32:
        result = get<std::uint32_t>(rhs) == get<std::uint32_t>(lhs);
        break;
      case TypeId::UInt64:
        result = get<std::uint64_t>(rhs) == get<std::uint64_t>(lhs);
        break;
      case TypeId::Float:
        result = get<float>(rhs) == get<float>(lhs);
        break;
      case TypeId::Double:
        result = get<double>(rhs) == get<double>(lhs);
        break;
      case TypeId::String:
        result = std::strcmp(get<const char*>(rhs), get<const char*>(lhs)) == 0;
        break;
      case TypeId::TripleInt32:
        result = get<0>(get<TripleBaseType<std::int32_t>>(rhs)) ==
                     get<0>(get<TripleBaseType<std::int32_t>>(lhs)) &&
                 get<1>(get<TripleBaseType<std::int32_t>>(rhs)) ==
                     get<1>(get<TripleBaseType<std::int32_t>>(lhs)) &&
                 get<2>(get<TripleBaseType<std::int32_t>>(rhs)) ==
                     get<2>(get<TripleBaseType<std::int32_t>>(lhs));
        break;
      case TypeId::TripleFloat:
        result = get<0>(get<TripleBaseType<float>>(rhs)) ==
                     get<0>(get<TripleBaseType<float>>(lhs)) &&
                 get<1>(get<TripleBaseType<float>>(rhs)) ==
                     get<1>(get<TripleBaseType<float>>(lhs)) &&
                 get<2>(get<TripleBaseType<float>>(rhs)) ==
                     get<2>(get<TripleBaseType<float>>(lhs));
        break;
      case TypeId::TripleDouble:
        result = get<0>(get<TripleBaseType<double>>(rhs)) ==
                     get<0>(get<TripleBaseType<double>>(lhs)) &&
                 get<1>(get<TripleBaseType<double>>(rhs)) ==
                     get<1>(get<TripleBaseType<double>>(lhs)) &&
                 get<2>(get<TripleBaseType<double>>(rhs)) ==
                     get<2>(get<TripleBaseType<double>>(lhs));
        break;
      case TypeId::Invalid:
        result = true;
        break;
    }
    return result;
  }

 private:
  constexpr static const char* name_[] = {
      "int32_t",       "int64_t",        "uint32_t", "uint64_t",
      "float",         "double",         "string",   "Triple<int32_t>",
      "Triple<float>", "Triple<double>", "Invalid"};
  static_assert(sizeof(name_) / sizeof(name_[0]) ==
                    static_cast<std::size_t>(TypeId::MAX_TYPE) + 1,
                "name_ size not match");
};

}  // namespace base
}  // namespace coriander
