/**
 * @file property_stream.h
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-07-23
 *
 * Copyright 2023 savent_gate
 *
 */
#pragma once

#include <cstdlib>
#include <cstring>
#include <memory>
#include <string>

#include "coriander/base/property.h"

namespace coriander {
namespace base {

struct PropertyTextStream : public Property {
  PropertyTextStream(Property&& p) noexcept : Property{std::move(p)} {}
  explicit PropertyTextStream() noexcept
      : Property{Type{Invalid{}}, ParamId::Unknow} {}

  ~PropertyTextStream() noexcept = default;

  template <typename T>
  friend T& operator>>(T& is, PropertyTextStream& p) {
    auto& value = p.value();
    std::string type;

    is >> p.m_dynamic_name >> type >> p.m_dynamic_desc;

    p.m_name = p.m_dynamic_name.c_str();
    p.m_desc = p.m_dynamic_desc.c_str();

    switch (TypeHelper::type(type.c_str())) {
      case TypeId::Int32: {
        int32_t v;
        is >> v;
        value = v;
        break;
      }
      case TypeId::Int64: {
        int64_t v;
        is >> v;
        value = v;
        break;
      }
      case TypeId::UInt32: {
        uint32_t v;
        is >> v;
        value = v;
        break;
      }
      case TypeId::UInt64: {
        uint64_t v;
        is >> v;
        value = v;
        break;
      }
      case TypeId::Float: {
        float v;
        is >> v;
        value = v;
        break;
      }
      case TypeId::Double: {
        double v;
        is >> v;
        value = v;
        break;
      }
      case TypeId::String: {
        is >> p.m_dynamic_str_value;
        value = p.m_dynamic_str_value.c_str();
        break;
      }
      case TypeId::TripleInt32: {
        int32_t v1, v2, v3;
        is >> v1 >> v2 >> v3;
        value = TripleBaseType<int32_t>{v1, v2, v3};
        break;
      }
      case TypeId::TripleFloat: {
        float v1, v2, v3;
        is >> v1 >> v2 >> v3;
        value = TripleBaseType<float>{v1, v2, v3};
        break;
      }
      case TypeId::TripleDouble: {
        double v1, v2, v3;
        is >> v1 >> v2 >> v3;
        value = TripleBaseType<double>{v1, v2, v3};
        break;
      }
      case TypeId::Invalid: {
        value = Invalid{};
        break;
      }
    }
    return is;
  }

  template <typename T>
  friend T& operator<<(T& os, const Property& p) {
    auto& value = p.value();
    auto id = TypeHelper::type(value);

    os << p.name() << " " << TypeHelper::name(TypeHelper::type(p.value()))
       << " " << p.desc() << " ";
    switch (id) {
      case TypeId::Int32:
        os << get<int32_t>(value);
        break;
      case TypeId::Int64:
        os << get<int64_t>(value);
        break;
      case TypeId::UInt32:
        os << get<uint32_t>(value);
        break;
      case TypeId::UInt64:
        os << get<uint64_t>(value);
        break;
      case TypeId::Float:
        os << get<float>(value);
        break;
      case TypeId::Double:
        os << get<double>(value);
        break;
      case TypeId::String:
        os << std::string{get<const char*>(value)};
        break;
      case TypeId::TripleInt32: {
        auto triple = get<TripleBaseType<int32_t>>(value);
        os << get<0>(triple) << " " << get<1>(triple) << " " << get<2>(triple);
        break;
      }
      case TypeId::TripleFloat: {
        auto triple = get<TripleBaseType<float>>(value);
        os << get<0>(triple) << " " << get<1>(triple) << " " << get<2>(triple);
        break;
      }
      case TypeId::TripleDouble: {
        auto triple = get<TripleBaseType<double>>(value);
        os << get<0>(triple) << " " << get<1>(triple) << " " << get<2>(triple);
        break;
      }
      case TypeId::Invalid: {
        os << "Invalid";
        break;
      }
    }
    return os;
  }

 private:
  std::string m_dynamic_name;
  std::string m_dynamic_desc;
  std::string m_dynamic_str_value;
};

namespace detail {

#pragma pack(push, 1)
struct PropertyBinaryStreamHeader {
  std::uint8_t type;
  std::uint8_t name_size;
  std::uint8_t desc_size;
  std::uint8_t value_size;
};
#pragma pack(pop)
}  // namespace detail

struct PropertyBinaryStream : public Property {
  PropertyBinaryStream(Property&& p) noexcept : Property{std::move(p)} {}
  PropertyBinaryStream(const Property& p) noexcept : Property{p} {}
  explicit PropertyBinaryStream() noexcept
      : Property{Type{Invalid{}}, ParamId::Unknow} {}

  ~PropertyBinaryStream() noexcept = default;

  template <typename T>
  friend T& operator>>(T& is, PropertyBinaryStream& p) {
    auto& value = p.value();
    detail::PropertyBinaryStreamHeader header = {0};
    uint64_t triple_buf[3] = {0};
    char* triple_buf_ptr = reinterpret_cast<char*>(triple_buf);

    is.read(reinterpret_cast<char*>(&header), sizeof(header));
    p.m_dynamic_name.resize(header.name_size);
    p.m_dynamic_desc.resize(header.desc_size);
    is.read(p.m_dynamic_name.data(), header.name_size);
    is.read(p.m_dynamic_desc.data(), header.desc_size);

    p.m_name = p.m_dynamic_name.c_str();
    p.m_desc = p.m_dynamic_desc.c_str();

    switch (TypeHelper::type(header.type)) {
      case TypeId::Int32: {
        int32_t v;
        is.read(reinterpret_cast<char*>(&v), sizeof(v));
        value = v;
        break;
      }
      case TypeId::Int64: {
        int64_t v;
        is.read(reinterpret_cast<char*>(&v), sizeof(v));
        value = v;
        break;
      }
      case TypeId::UInt32: {
        uint32_t v;
        is.read(reinterpret_cast<char*>(&v), sizeof(v));
        value = v;
        break;
      }
      case TypeId::UInt64: {
        uint64_t v;
        is.read(reinterpret_cast<char*>(&v), sizeof(v));
        value = v;
        break;
      }
      case TypeId::Float: {
        float v;
        is.read(reinterpret_cast<char*>(&v), sizeof(v));
        value = v;
        break;
      }
      case TypeId::Double: {
        double v;
        is.read(reinterpret_cast<char*>(&v), sizeof(v));
        value = v;
        break;
      }
      case TypeId::String: {
        p.m_dynamic_str_value.resize(header.value_size);
        is.read(p.m_dynamic_str_value.data(), header.value_size);
        value = p.m_dynamic_str_value.c_str();
        break;
      }
      case TypeId::TripleInt32: {
        is.read(triple_buf_ptr, sizeof(uint32_t) * 3);
        value = TripleBaseType<int32_t>{
            *reinterpret_cast<uint32_t*>(triple_buf_ptr),
            *reinterpret_cast<uint32_t*>(triple_buf_ptr + sizeof(uint32_t)),
            *reinterpret_cast<uint32_t*>(triple_buf_ptr +
                                         sizeof(uint32_t) * 2)};
        break;
      }
      case TypeId::TripleFloat: {
        is.read(triple_buf_ptr, sizeof(float) * 3);
        value = TripleBaseType<float>{
            *reinterpret_cast<float*>(triple_buf_ptr),
            *reinterpret_cast<float*>(triple_buf_ptr + sizeof(float)),
            *reinterpret_cast<float*>(triple_buf_ptr + sizeof(float) * 2)};
        break;
      }
      case TypeId::TripleDouble: {
        is.read(triple_buf_ptr, sizeof(double) * 3);
        value = TripleBaseType<double>{
            *reinterpret_cast<double*>(triple_buf_ptr),
            *reinterpret_cast<double*>(triple_buf_ptr + sizeof(double)),
            *reinterpret_cast<double*>(triple_buf_ptr + sizeof(double) * 2)};
        break;
      }
      case TypeId::Invalid: {
        value = Invalid{};
        break;
      }
    }
    return is;
  }

  template <typename T>
  friend T& operator<<(T& os, const PropertyBinaryStream& p) {
    auto& value = p.value();
    auto type = TypeHelper::type(value);
    detail::PropertyBinaryStreamHeader header = {
        static_cast<uint8_t>(value.index()),
        static_cast<uint8_t>(std::strlen(p.name())),
        static_cast<uint8_t>(std::strlen(p.desc())), 0};
    const char* value_ptr = nullptr;
    uint64_t triple_buf[3] = {0};
    char* triple_buf_ptr = reinterpret_cast<char*>(triple_buf);

    switch (type) {
      case TypeId::Int32:
        header.value_size = sizeof(int32_t);
        value_ptr = reinterpret_cast<const char*>(&get<int32_t>(value));
        break;
      case TypeId::Int64:
        header.value_size = sizeof(int64_t);
        value_ptr = reinterpret_cast<const char*>(&get<int64_t>(value));
        break;
      case TypeId::UInt32:
        header.value_size = sizeof(uint32_t);
        value_ptr = reinterpret_cast<const char*>(&get<uint32_t>(value));
        break;
      case TypeId::UInt64:
        header.value_size = sizeof(uint64_t);
        value_ptr = reinterpret_cast<const char*>(&get<uint64_t>(value));
        break;
      case TypeId::Float:
        header.value_size = sizeof(float);
        value_ptr = reinterpret_cast<const char*>(&get<float>(value));
        break;
      case TypeId::Double:
        header.value_size = sizeof(double);
        value_ptr = reinterpret_cast<const char*>(&get<double>(value));
        break;
      case TypeId::String:
        header.value_size = std::strlen(get<const char*>(value));
        value_ptr = get<const char*>(value);
        break;
      case TypeId::TripleInt32: {
        auto& triple = get<TripleBaseType<int32_t>>(value);
        header.value_size = sizeof(uint32_t) * 3;
        std::memcpy(triple_buf_ptr, &get<0>(triple), sizeof(uint32_t));
        std::memcpy(triple_buf_ptr + sizeof(uint32_t), &get<1>(triple),
                    sizeof(uint32_t));
        std::memcpy(triple_buf_ptr + sizeof(uint32_t) * 2, &get<2>(triple),
                    sizeof(uint32_t));
        value_ptr = triple_buf_ptr;
        break;
      }
      case TypeId::TripleFloat: {
        auto& triple = get<TripleBaseType<float>>(value);
        header.value_size = sizeof(float) * 3;
        std::memcpy(triple_buf_ptr, &get<0>(triple), sizeof(float));
        std::memcpy(triple_buf_ptr + sizeof(float), &get<1>(triple),
                    sizeof(float));
        std::memcpy(triple_buf_ptr + sizeof(float) * 2, &get<2>(triple),
                    sizeof(float));
        value_ptr = triple_buf_ptr;
        break;
      }
      case TypeId::TripleDouble: {
        auto& triple = get<TripleBaseType<double>>(value);
        header.value_size = sizeof(double) * 3;
        std::memcpy(triple_buf_ptr, &get<0>(triple), sizeof(double));
        std::memcpy(triple_buf_ptr + sizeof(double), &get<1>(triple),
                    sizeof(double));
        std::memcpy(triple_buf_ptr + sizeof(double) * 2, &get<2>(triple),
                    sizeof(double));
        value_ptr = triple_buf_ptr;
        break;
      }
      case TypeId::Invalid:
        return os;
    }

    os.write(reinterpret_cast<const char*>(&header), sizeof(header));
    os.write(p.name(), header.name_size);
    os.write(p.desc(), header.desc_size);
    os.write(value_ptr, header.value_size);
    return os;
  }

  bool operator==(const PropertyBinaryStream& other) const noexcept {
    return Property::operator==(*(Property*)&other);
  }

 private:
  std::string m_dynamic_name;
  std::string m_dynamic_desc;
  std::string m_dynamic_str_value;
};

using PropertyDebugStream = PropertyTextStream;
using PropertyStream = PropertyBinaryStream;

}  // namespace base
}  // namespace coriander
