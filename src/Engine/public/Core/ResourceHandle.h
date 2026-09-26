#pragma once

#include "Core/Assert.h"

#include <cstddef>
#include <cstdint>
#include <functional>
#include <limits>

// Bit layouts for ResourceHandle: index (slot) + generation (reuse counter).
struct HandleLayout32
{
	using ValueType                           = std::uint32_t;
	static constexpr unsigned kIndexBits      = 20; // ~1M slots
	static constexpr unsigned kGenerationBits = 12; // 4095 reuses per slot
};

struct HandleLayout64
{
	using ValueType                           = std::uint64_t;
	static constexpr unsigned kIndexBits      = 32;
	static constexpr unsigned kGenerationBits = 32;
};

// Project-wide choice: switching every handle to 64 bits is a one-line change here.
using HandleLayout = HandleLayout32;

// Lightweight reference to a resource owned elsewhere (textures, fonts, ...): index + generation.
// Generation 0 is reserved so a valid handle never equals the null (default) value.
// The raw bits never leave the type: use Index(), Generation(), IsNull(). Never serialize a handle.
template <typename Tag, typename Layout = HandleLayout>
class ResourceHandle
{
	using ValueType = typename Layout::ValueType;
	static_assert(Layout::kIndexBits + Layout::kGenerationBits == std::numeric_limits<ValueType>::digits,
	              "the layout must use every bit of its value type");

public:
	static constexpr ValueType kMaxIndex      = (ValueType{ 1 } << Layout::kIndexBits) - 1;
	static constexpr ValueType kMaxGeneration = (ValueType{ 1 } << Layout::kGenerationBits) - 1;

	constexpr ResourceHandle() noexcept = default;

	[[nodiscard]] static constexpr ResourceHandle Create(ValueType index, ValueType generation) noexcept
	{
		MYRIAS_ASSERT(index <= kMaxIndex, "ResourceHandle: index out of range");
		MYRIAS_ASSERT(generation != 0 && generation <= kMaxGeneration, "ResourceHandle: generation out of range");
		ResourceHandle handle;
		handle.value = static_cast<ValueType>((generation << Layout::kIndexBits) | index);
		return handle;
	}

	[[nodiscard]] constexpr ValueType Index() const noexcept { return value & kMaxIndex; }
	[[nodiscard]] constexpr ValueType Generation() const noexcept { return value >> Layout::kIndexBits; }
	[[nodiscard]] constexpr bool      IsNull() const noexcept { return value == 0; }

	friend constexpr bool operator==(ResourceHandle, ResourceHandle) noexcept = default;

private:
	friend struct std::hash<ResourceHandle>;

	ValueType value = 0;
};

template <typename Tag, typename Layout>
struct std::hash<ResourceHandle<Tag, Layout>>
{
	[[nodiscard]] std::size_t operator()(ResourceHandle<Tag, Layout> handle) const noexcept
	{
		return std::hash<typename Layout::ValueType>{}(handle.value);
	}
};
