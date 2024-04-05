//=================================================================================================
// Copyright (C) 2023-2024 HEPHAESTUS Contributors
//=================================================================================================

#pragma once

#include <hephaestus/utils/exception.h>
#include <range/v3/range/conversion.hpp>

#include "hephaestus/serdes/protobuf/buffers.h"
#include "hephaestus/serdes/protobuf/protobuf_internal.h"
#include "hephaestus/serdes/type_info.h"
#include "hephaestus/utils/utils.h"

namespace heph::serdes::protobuf {

template <class T>
[[nodiscard]] auto serialize(const T& data) -> std::vector<std::byte>;

template <class T>
auto deserialize(std::span<const std::byte> buffer, T& data) -> void;

/// Create the type info for the serialized type associated with `T`.
template <class T>
[[nodiscard]] auto getTypeInfo() -> TypeInfo;

// --- Implementation ---
template <class T>
auto serialize(const T& data) -> std::vector<std::byte> {
  SerializerBuffer buffer{};
  internal::toProtobuf(buffer, data);
  return std::move(buffer).exctractSerializedData();
}

template <class T>
auto deserialize(std::span<const std::byte> buffer, T& data) -> void {
  DeserializerBuffer des_buffer{ buffer };
  internal::fromProtobuf(des_buffer, data);
}

template <class T>
auto getTypeInfo() -> TypeInfo {
  using Proto = ProtoAssociation<T>::Type;
  auto proto_descriptor = Proto::descriptor();
  auto file_descriptor = internal::buildFileDescriptorSet(proto_descriptor).SerializeAsString();

  std::vector<std::byte> schema(file_descriptor.size());
  std::transform(file_descriptor.begin(), file_descriptor.end(), schema.begin(),
                 [](char c) { return static_cast<std::byte>(c); });
  return { .name = proto_descriptor->full_name(),
           .schema = schema,
           .serialization = TypeInfo::Serialization::PROTOBUF,
           .original_type = utils::getTypeName<T>() };
}

}  // namespace heph::serdes::protobuf
