#include "AssetManager.h"

#include <iostream>
#include <utility>

bool AssetManager::LoadFont(const std::string& id, const std::string& filename)
{
	sf::Font font;
	if (!font.openFromFile(filename))
	{
		std::cerr << "Failed to load font: " << filename << '\n';
		return false;
	}
	fonts.emplace(id, std::move(font));
	return true;
}

const sf::Font& AssetManager::GetFont(const std::string& id) const
{
	return fonts.at(id);
}

bool AssetManager::LoadTexture(const std::string& id, const std::string& filename)
{
	sf::Texture texture;
	if (!texture.loadFromFile(filename))
	{
		std::cerr << "Failed to load texture: " << filename << '\n';
		return false;
	}
	texture.setSmooth(true);
	textures.emplace(id, std::move(texture));
	return true;
}

const sf::Texture* AssetManager::GetTexture(const std::string& id) const
{
	const auto it = textures.find(id);
	return it != textures.end() ? &it->second : nullptr;
}
