#include "Renderer/AssetManager.h"

#include <iostream>
#include <utility>

bool AssetManager::LoadFont(const std::string& id, const std::string& filename)
{
	// Check before opening the file: a duplicate would be loaded only to be discarded.
	if (fonts.contains(id))
	{
		std::cerr << "Font id already loaded, keeping the existing one: " << id << '\n';
		return false;
	}

	sf::Font font;
	if (!font.openFromFile(filename))
	{
		std::cerr << "Failed to load font: " << filename << '\n';
		return false;
	}
	fonts.emplace(id, std::move(font));
	return true;
}

bool AssetManager::LoadTexture(const std::string& id, const std::string& filename)
{
	// Check before loading: a duplicate would be uploaded to the GPU only to be discarded.
	if (textures.contains(id))
	{
		std::cerr << "Texture id already loaded, keeping the existing one: " << id << '\n';
		return false;
	}

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

bool AssetManager::HasFont(std::string_view id) const noexcept
{
	return FindFont(id) != nullptr;
}

bool AssetManager::HasTexture(std::string_view id) const noexcept
{
	return FindTexture(id) != nullptr;
}

const sf::Font* AssetManager::FindFont(std::string_view id) const noexcept
{
	const auto it = fonts.find(id);
	return it != fonts.end() ? &it->second : nullptr;
}

const sf::Texture* AssetManager::FindTexture(std::string_view id) const noexcept
{
	const auto it = textures.find(id);
	return it != textures.end() ? &it->second : nullptr;
}
