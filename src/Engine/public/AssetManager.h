#pragma once

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <string>
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

	bool LoadFont(const std::string& id, const std::string& filename);
	const sf::Font& GetFont(const std::string& id) const;
	bool LoadTexture(const std::string& id, const std::string& filename);
	
	const sf::Texture* GetTexture(const std::string& id) const;

private:
	AssetManager() = default;

	std::unordered_map<std::string, sf::Font>    fonts;
	std::unordered_map<std::string, sf::Texture> textures;
};
