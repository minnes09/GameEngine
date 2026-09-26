#include "Renderer/AssetManager.h"

#include <gtest/gtest.h>

#include <string_view>

// AssetManager is a singleton: ids are unique to these tests. Nothing is loaded, so no GPU is needed.

TEST(AssetManager, MissingFontLookupsDoNotThrow)
{
	const auto& assets = AssetManager::Get();
	EXPECT_FALSE(assets.HasFont("AssetManagerTests.missing-font"));
	EXPECT_EQ(assets.FindFont("AssetManagerTests.missing-font"), nullptr);
}

TEST(AssetManager, MissingTextureLookupsReturnNull)
{
	const auto& assets = AssetManager::Get();
	EXPECT_FALSE(assets.HasTexture("AssetManagerTests.missing-texture"));
	EXPECT_EQ(assets.FindTexture("AssetManagerTests.missing-texture"), nullptr);
}

TEST(AssetManager, LookupAcceptsNonTerminatedView)
{
	// A view into a larger buffer: the lookup must use the view's length, not a terminator.
	constexpr std::string_view kBuffer = "AssetManagerTests.viewXYZ";
	const std::string_view     id      = kBuffer.substr(0, kBuffer.size() - 3);
	EXPECT_EQ(AssetManager::Get().FindTexture(id), nullptr);
}

TEST(AssetManager, LoadingMissingFilesFails)
{
	auto& assets = AssetManager::Get();
	EXPECT_FALSE(assets.LoadFont("AssetManagerTests.bad-font", "AssetManagerTests/does-not-exist.ttf"));
	EXPECT_FALSE(assets.LoadTexture("AssetManagerTests.bad-texture", "AssetManagerTests/does-not-exist.png"));
	EXPECT_FALSE(assets.HasFont("AssetManagerTests.bad-font"));
	EXPECT_FALSE(assets.HasTexture("AssetManagerTests.bad-texture"));
}
