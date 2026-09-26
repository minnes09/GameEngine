#include "Core/StringId.h"

#include <gtest/gtest.h>

#include <string>
#include <unordered_set>

// Official FNV-1a 64-bit test vectors, checked at compile time.
static_assert(StringId{ "" }.Value() == 0xcbf29ce484222325ull);
static_assert(StringId{ "a" }.Value() == 0xaf63dc4c8601ec8cull);
static_assert(StringId{ "foobar" }.Value() == 0x85944171f73967e8ull);
static_assert(StringId{}.IsNull());

TEST(StringId, RuntimeHashMatchesCompileTimeHash)
{
	constexpr StringId  kCompileTime{ "foobar" };
	const std::string   name = "foobar";
	const StringId      runtime{ name };
	EXPECT_EQ(runtime, kCompileTime);
	EXPECT_EQ(runtime.Value(), 0x85944171f73967e8ull);
}

TEST(StringId, DefaultIsNullAndNamedIsNot)
{
	EXPECT_TRUE(StringId{}.IsNull());
	EXPECT_FALSE(StringId{ "Player" }.IsNull());
}

TEST(StringId, EqualityAndOrdering)
{
	constexpr StringId kA{ "Alpha" };
	constexpr StringId kB{ "Beta" };
	EXPECT_EQ(kA, StringId{ "Alpha" });
	EXPECT_NE(kA, kB);
	EXPECT_TRUE((kA < kB) != (kB < kA));
}

TEST(StringId, WorksAsUnorderedKey)
{
	std::unordered_set<StringId> ids{ StringId{ "One" }, StringId{ "Two" } };
	EXPECT_TRUE(ids.contains(StringId{ "One" }));
	EXPECT_FALSE(ids.contains(StringId{ "Three" }));
}

TEST(StringIdDebug, LookupReturnsRegisteredName)
{
	if constexpr (!MYRIAS_ASSERTS_ENABLED)
		GTEST_SKIP() << "the debug registry is compiled out";

	const std::string name = "StringIdTests.lookup";
	const StringId    id{ name }; // runtime construction registers the name
	EXPECT_EQ(StringIdDebug::Lookup(id), "StringIdTests.lookup");
}

TEST(StringIdDebug, CompileTimeIdsAreNotRegistered)
{
	constexpr StringId kNeverRegistered{ "StringIdTests.compile-time-only" };
	EXPECT_EQ(StringIdDebug::Lookup(kNeverRegistered), "<unknown>");
}

TEST(StringIdDebugDeathTest, CollisionAsserts)
{
	if constexpr (!MYRIAS_ASSERTS_ENABLED)
		GTEST_SKIP() << "the debug registry is compiled out";

	const StringId id{ std::string("StringIdTests.collision") };
	// Simulate a collision: same hash, different name.
	EXPECT_DEBUG_DEATH(StringIdDebug::Register(id, "StringIdTests.other-name"), "StringId collision");
}
