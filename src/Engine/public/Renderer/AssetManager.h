#pragma once

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <cstddef>
#include <functional>
#include <string>
#include <string_view>
#include <unordered_map>

class AssetManager
{
public:
	// Single-instance engine service (Meyers singleton): the sole instance lives inside Get().
	// Non-copyable / non-movable so it can only ever exist once.
	AssetManager(const AssetManager&)            = delete;
	AssetManager& operator=(const AssetManager&) = delete;
	AssetManager(AssetManager&&)                 = delete;
	AssetManager& operator=(AssetManager&&)      = delete;

	static AssetManager& Get()
	{
		static AssetManager instance;
		return instance;
	}

	// Return false if the file cannot be loaded or the id is already taken (the existing asset is kept).
	bool LoadFont(const std::string& id, const std::string& filename);
	bool LoadTexture(const std::string& id, const std::string& filename);

	// Lookups by string_view: no allocation, no throw. Safe to call every frame.
	[[nodiscard]] bool HasFont(std::string_view id) const noexcept;
	[[nodiscard]] bool HasTexture(std::string_view id) const noexcept;
	[[nodiscard]] const sf::Font*    FindFont(std::string_view id) const noexcept;
	[[nodiscard]] const sf::Texture* FindTexture(std::string_view id) const noexcept;

private:
	AssetManager() = default;

	// Transparent hash so find() accepts string_view without building a std::string.
	struct StringHash
	{
		using is_transparent = void;
		std::size_t operator()(std::string_view value) const noexcept { return std::hash<std::string_view>{}(value); }
	};

	template <typename T>
	using AssetMap = std::unordered_map<std::string, T, StringHash, std::equal_to<>>;

	AssetMap<sf::Font>    fonts;
	AssetMap<sf::Texture> textures;
};
