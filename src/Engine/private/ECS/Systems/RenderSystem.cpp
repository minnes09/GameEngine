#include "ECS/Systems/RenderSystem.h"

#include "AssetManager.h"
#include "ECS/Components/Renderable.h"
#include "ECS/Components/Text.h"
#include "ECS/Components/Transform.h"
#include "ECS/EntityManager.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <algorithm>
#include <cstdint>

namespace RenderSystem_Private
{
void locUpdateShape(sf::Shape& shape, const sf::Vector2f& position, const RenderableComponent& renderable,
                    const sf::Color& fillColor)
{
	shape.setPosition(position);
	shape.setFillColor(fillColor);
	shape.setOutlineColor(renderable.outlineColor);
	shape.setOutlineThickness(renderable.outlineThickness);
}

} // namespace RenderSystem_Private

void RenderSystem::Render(sf::RenderWindow& window)
{
	auto* renderablePool = EntityManager::Get().GetPool<RenderableComponent>();
	auto* transformPool = EntityManager::Get().GetPool<TransformComponent>();
	if (!renderablePool || !transformPool)
		return;

	renderShapes(transformPool, renderablePool, window);
	renderTexts(transformPool, renderablePool, window);
}

void RenderSystem::renderShapes(ComponentPool<TransformComponent>* transformPool,
                                ComponentPool<RenderableComponent>* renderablePool,
                                sf::RenderWindow& window)
{
	renderablePool->ForEach(
	    [&](EntityId entity, const RenderableComponent& renderable)
	    {
		    auto* transform = transformPool->Get(entity);
		    if (!transform)
			    return;

		    const sf::Color drawColor = renderable.color;

		    // Textured entities draw as a sprite scaled to `size`, tinted by its
		    // colour. Missing texture -> fall through to the shape.
		    if (renderable.textureId)
		    {
			    if (const sf::Texture* texture = AssetManager::Get().GetTexture(renderable.textureId))
			    {
				    sf::Sprite sprite(*texture);
				    const sf::Vector2u ts = texture->getSize();
				    if (ts.x > 0 && ts.y > 0)
					    sprite.setScale({renderable.size.x / static_cast<float>(ts.x),
					                     renderable.size.y / static_cast<float>(ts.y)});
				    sprite.setPosition(transform->position);
				    sprite.setColor(drawColor);
				    window.draw(sprite);
				    return;
			    }
		    }

		    switch (renderable.shape)
		    {
			    case Shape::Rectangle:
			    {
				    sf::RectangleShape shape(renderable.size);
				    RenderSystem_Private::locUpdateShape(shape, transform->position, renderable, drawColor);
				    window.draw(shape);
				    break;
			    }
			    case Shape::Circle:
			    {
				    sf::CircleShape shape(renderable.size.x * 0.5f);
				    RenderSystem_Private::locUpdateShape(shape, transform->position, renderable, drawColor);
				    window.draw(shape);
				    break;
			    }
		    }
	    });
}

void RenderSystem::renderTexts(ComponentPool<TransformComponent>* transformPool,
                               ComponentPool<RenderableComponent>* renderablePool,
                               sf::RenderWindow& window)
{
	// Text pass: drawn after shapes/sprites so labels sit on top. Centered inside the entity's
	// shape when it has one, otherwise on its position.
	auto* textPool = EntityManager::Get().GetPool<TextComponent>();
	if (!textPool)
		return;

	const sf::Font& font = AssetManager::Get().GetFont("default");
	textPool->ForEach(
	    [&](EntityId entity, const TextComponent& textComp)
	    {
		    const auto* transform = transformPool->Get(entity);
		    if (!transform)
			    return;

		    sf::Text text(font, textComp.text, textComp.characterSize);
		    text.setFillColor(textComp.color);
		    const sf::FloatRect bounds = text.getLocalBounds();
		    text.setOrigin(bounds.position + bounds.size * 0.5f);

		    sf::Vector2f center = transform->position;
		    if (const auto* renderable = renderablePool->Get(entity))
			    // size is the full box for both shapes (a circle is inscribed in it), so its half is the center.
			    center += renderable->size * 0.5f;
		    text.setPosition(center);
		    window.draw(text);
	    });
}
