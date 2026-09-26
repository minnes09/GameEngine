#include "Renderer/Renderer.h"

#include "Renderer/AssetManager.h"
#include "SfmlConversions.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <string>
#include <string_view>
#include <vector>

namespace SfmlRenderer_Private
{
namespace
{
constexpr std::string_view kDefaultFontId = "default";

void ApplyStyle(sf::Shape& shape, const ShapeDrawCommand& command)
{
	shape.setPosition(ToSf(command.position));
	shape.setFillColor(ToSf(command.fillColor));
	shape.setOutlineColor(ToSf(command.outlineColor));
	shape.setOutlineThickness(command.outlineThickness);
}
} // namespace
} // namespace SfmlRenderer_Private

// Reusable SFML objects: shapes are reconfigured per command, texts live in a pool that only
// grows during warm-up. Frames whose texts did not change do not allocate; a changed text
// allocates while it is re-encoded for SFML.
struct Renderer::Backend
{
	struct TextSlot
	{
		explicit TextSlot(const sf::Font& font) : text(font) {}

		sf::Text    text;
		std::string cachedString; // what `text` currently holds: setString only when it changes
		unsigned    cachedSize = 0;
	};

	sf::RectangleShape    rectangle;
	sf::CircleShape       circle;
	std::vector<TextSlot> textPool;
	const sf::Font*       textFont = nullptr;

	void DrawShape(sf::RenderTarget& target, const AssetManager& assets, const ShapeDrawCommand& command);
	void DrawTexts(sf::RenderTarget& target, const sf::Font& font, const RenderQueue& queue);
};

void Renderer::Backend::DrawShape(sf::RenderTarget& target, const AssetManager& assets, const ShapeDrawCommand& command)
{
	if (command.textureId)
	{
		if (const sf::Texture* texture = assets.FindTexture(command.textureId))
		{
			sf::Sprite         sprite(*texture);
			const sf::Vector2u textureSize = texture->getSize();
			if (textureSize.x > 0 && textureSize.y > 0)
				sprite.setScale({ command.size.x / static_cast<float>(textureSize.x),
				                  command.size.y / static_cast<float>(textureSize.y) });
			sprite.setPosition(ToSf(command.position));
			sprite.setColor(ToSf(command.fillColor));
			target.draw(sprite);
			return;
		}
		// Missing texture: fall through to the shape.
	}

	switch (command.kind)
	{
		case ShapeKind::Rectangle:
			rectangle.setSize(ToSf(command.size));
			SfmlRenderer_Private::ApplyStyle(rectangle, command);
			target.draw(rectangle);
			break;
		case ShapeKind::Circle:
			circle.setRadius(command.size.x * 0.5f);
			SfmlRenderer_Private::ApplyStyle(circle, command);
			target.draw(circle);
			break;
	}
}

void Renderer::Backend::DrawTexts(sf::RenderTarget& target, const sf::Font& font, const RenderQueue& queue)
{
	if (textFont != &font)
	{
		textPool.clear();
		textFont = &font;
	}
	while (textPool.size() < queue.TextCount())
		textPool.emplace_back(font);

	for (std::size_t i = 0; i < queue.TextCount(); ++i)
	{
		const TextDrawCommand command = queue.Text(i);
		TextSlot&              slot    = textPool[i];

		if (slot.cachedString != command.text || slot.cachedSize != command.characterSize)
		{
			slot.cachedString.assign(command.text);
			slot.cachedSize = command.characterSize;
			slot.text.setString(sf::String::fromUtf8(command.text.begin(), command.text.end()));
			slot.text.setCharacterSize(command.characterSize);
			const sf::FloatRect bounds = slot.text.getLocalBounds();
			slot.text.setOrigin(bounds.position + bounds.size * 0.5f);
		}

		slot.text.setFillColor(ToSf(command.color));
		slot.text.setPosition(ToSf(command.center));
		target.draw(slot.text);
	}
}

Renderer::Renderer(sf::RenderTarget& target, const AssetManager& assets)
    : target(target)
    , assets(assets)
    , backend(std::make_unique<Backend>())
{
}

Renderer::~Renderer() noexcept = default;

void Renderer::BeginFrame(RenderColor clearColor)
{
	queue.Clear();
	target.clear(ToSf(clearColor));
}

void Renderer::EndFrame()
{
	for (const ShapeDrawCommand& command : queue.Shapes())
		backend->DrawShape(target, assets, command);

	if (queue.TextCount() == 0)
		return;

	// No default font: skip the text pass instead of throwing mid-frame.
	if (const sf::Font* font = assets.FindFont(SfmlRenderer_Private::kDefaultFontId))
		backend->DrawTexts(target, *font, queue);
}
