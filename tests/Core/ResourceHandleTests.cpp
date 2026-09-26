#include "Core/ResourceHandle.h"

#include <gtest/gtest.h>

#include <functional>
#include <type_traits>

namespace
{
struct TestTagA;
struct TestTagB;
using HandleA   = ResourceHandle<TestTagA>;
using HandleB   = ResourceHandle<TestTagB>;
using HandleA64 = ResourceHandle<TestTagA, HandleLayout64>;
} // namespace

// Size alarms: changing a layout must be a deliberate decision.
static_assert(sizeof(HandleA) == 4);
static_assert(sizeof(HandleA64) == 8);

// Different tags never convert into each other.
static_assert(!std::is_convertible_v<HandleA, HandleB>);
static_assert(!std::is_constructible_v<HandleA, HandleB>);

// Usable at compile time.
static_assert(HandleA{}.IsNull());
static_assert(HandleA::Create(3, 1).Index() == 3);
static_assert(HandleA::Create(3, 1).Generation() == 1);

TEST(ResourceHandle, DefaultIsNullAndCreatedIsNot)
{
	EXPECT_TRUE(HandleA{}.IsNull());
	EXPECT_FALSE(HandleA::Create(0, 1).IsNull());
}

TEST(ResourceHandle, PackUnpackAtLimits)
{
	const HandleA handle = HandleA::Create(HandleA::kMaxIndex, HandleA::kMaxGeneration);
	EXPECT_EQ(handle.Index(), (1u << 20) - 1);
	EXPECT_EQ(handle.Generation(), (1u << 12) - 1);

	const HandleA64 wide = HandleA64::Create(HandleA64::kMaxIndex, HandleA64::kMaxGeneration);
	EXPECT_EQ(wide.Index(), 0xFFFFFFFFull);
	EXPECT_EQ(wide.Generation(), 0xFFFFFFFFull);
}

TEST(ResourceHandle, EqualityAndHash)
{
	const HandleA first  = HandleA::Create(7, 2);
	const HandleA same   = HandleA::Create(7, 2);
	const HandleA reused = HandleA::Create(7, 3); // same slot, newer generation

	EXPECT_EQ(first, same);
	EXPECT_NE(first, reused);
	EXPECT_EQ(std::hash<HandleA>{}(first), std::hash<HandleA>{}(same));
}
