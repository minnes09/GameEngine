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
	m_fonts.emplace(id, std::move(font));
	return true;
}

const sf::Font& AssetManager::GetFont(const std::string& id) const
{
	return m_fonts.at(id);
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
	m_textures.emplace(id, std::move(texture));
	return true;
}

const sf::Texture* AssetManager::GetTexture(const std::string& id) const
{
	const auto it = m_textures.find(id);
	return it != m_textures.end() ? &it->second : nullptr;
}
