/**
 * @file parameters.cc
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-08-01
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "coriander/parameters.h"

#include <cstdlib>
#include <list>
#include <sstream>

#include "coriander/base/const_hash.h"
#include "coriander/base/property_stream.h"

namespace {
using namespace coriander;
using namespace coriander::base;

struct HeaderItem {
  uint16_t block_size;
  uint16_t block_offset;
  uint32_t block_checksum;
};

struct Header {
  uint32_t magic;
  uint32_t version;
  uint32_t checksum;
  uint32_t block_count;
  HeaderItem items[0];
};

struct HeaderItemVisitor : IPropertyVisitor {
  virtual void visit(const Property& property) noexcept override {
    PropertyBinaryStream stream{property};
    std::ostringstream ss{std::ios::binary};
    HeaderItem item;

    ss << stream;

    auto str = ss.str();
    auto block = std::make_unique<uint8_t[]>(str.size());
    std::memcpy(block.get(), str.data(), str.size());
    item.block_size = str.size();
    item.block_checksum = string_hash(str);
    item.block_offset = offset;

    offset += item.block_size;
    items.push_back(item);
    blocks.push_back(std::move(block));
  }
  std::list<HeaderItem> items;
  std::list<std::unique_ptr<uint8_t[]>> blocks;
  uint16_t offset = 0;
};

}  // namespace
namespace coriander {

std::span<uint8_t> ParameterMemoryMapper::map(
    const ParameterBase* param) noexcept {
  if (mMapped) {
    return std::span<uint8_t>{reinterpret_cast<uint8_t*>(mMapped.get()),
                              mMappedSize};
  }

  HeaderItemVisitor visitor;
  param->accept(visitor);

  Header header;
  header.magic = "PARAM"_hash;
  header.version = 1;
  header.block_count = visitor.items.size();
  header.checksum = 0;
  for (auto& item : visitor.items) {
    header.checksum += item.block_checksum;
  }

  auto headerSize = sizeof(Header) + sizeof(HeaderItem) * header.block_count;
  auto payloadSize = visitor.offset;
  auto totalSize = headerSize + payloadSize;

  auto mapped = std::make_unique<uint8_t[]>(totalSize);
  std::memcpy(mapped.get(), &header, sizeof(Header));
  auto offset = sizeof(Header);
  for (auto& item : visitor.items) {
    std::memcpy(mapped.get() + offset, &item, sizeof(HeaderItem));
    offset += sizeof(HeaderItem);
  }

  auto itemIter = visitor.items.begin();
  auto blockIter = visitor.blocks.begin();
  for (; itemIter != visitor.items.end() && blockIter != visitor.blocks.end();
       ++itemIter, ++blockIter) {
    std::memcpy(mapped.get() + offset, blockIter->get(), itemIter->block_size);
    offset += itemIter->block_size;
  }

  mMapped = std::move(mapped);
  mMappedSize = totalSize;

  return std::span<uint8_t>{mMapped.get(), mMappedSize};
}

void ParameterMemoryMapper::unmap() noexcept {
  mMapped = nullptr;
  mMappedSize = 0;
}

bool ParameterMemoryMapper::isValid(std::span<uint8_t> data) const noexcept {
  auto header = reinterpret_cast<const Header*>(data.data());
  auto payloadOffset =
      sizeof(Header) + sizeof(HeaderItem) * header->block_count;
  // header checks
  if (header->magic != "PARAM"_hash) {
    return false;
  }
  if (header->version != 1) {
    return false;
  }

  // header items checks
  uint16_t offset = 0;
  for (unsigned idx = 0; idx < header->block_count; ++idx) {
    auto& item = header->items[idx];

    if (item.block_offset != offset) {
      return false;
    }

    auto block =
        data.subspan(payloadOffset + item.block_offset, item.block_size);
    if (item.block_checksum !=
        base::string_hash(reinterpret_cast<char*>(block.data()),
                          block.size())) {
      return false;
    }

    offset += item.block_size;
  }

  if (offset + sizeof(Header) + sizeof(HeaderItem) * header->block_count !=
      data.size()) {
    return false;
  }
  return true;
}

bool ParameterMemoryMapper::recovery(std::span<uint8_t> data,
                                     ParameterBase* param) const noexcept {
  if (isValid(data) == false) {
    return false;
  }

  auto header = reinterpret_cast<const Header*>(data.data());
  auto payloadOffset =
      sizeof(Header) + sizeof(HeaderItem) * header->block_count;

  for (unsigned idx = 0; idx < header->block_count; ++idx) {
    auto& item = header->items[idx];
    auto block =
        data.subspan(payloadOffset + item.block_offset, item.block_size);
    std::istringstream is(
        std::string{reinterpret_cast<const char*>(block.data()), block.size()},
        std::ios::binary);
    PropertyBinaryStream pbs;
    is >> pbs;

    base::TypeId t = base::TypeHelper::type(pbs.value().index());
    if (t == base::TypeId::Invalid) {
      return false;
    }

    if (param->has(pbs.name())) {
      param->remove(pbs.name());
    }
    param->add(pbs);
  }

  return true;
}  // namespace coriander
}  // namespace coriander
