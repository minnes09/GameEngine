#pragma once

#include <cstdint>

// Deprecated: replaced by EntityId (64-bit layout, ADR-0004); removed at the end of the ECS redesign.
enum class DEPRECATED_EntityId : std::uint32_t {};
