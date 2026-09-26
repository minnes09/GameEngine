#pragma once

// Single point where the backend's math/color types enter the backend-neutral renderer API.
// When Core gets its own Vec2/Color, only these aliases (and ToSf in the backend) change.

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>

using RenderVec2  = sf::Vector2f;
using RenderColor = sf::Color;
