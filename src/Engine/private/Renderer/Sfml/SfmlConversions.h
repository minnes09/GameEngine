#pragma once

// The only place where renderer types are converted to SFML types. Today the render types are
// SFML aliases, so these are identities; when Core gets its own Vec2/Color only this file changes.

#include "Renderer/RenderTypes.h"

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>

[[nodiscard]] inline sf::Vector2f ToSf(RenderVec2 value) noexcept
{
	return value;
}

[[nodiscard]] inline sf::Color ToSf(RenderColor value) noexcept
{
	return value;
}
