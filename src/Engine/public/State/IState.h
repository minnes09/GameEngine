#pragma once

#include <SFML/Graphics/Color.hpp>

#include <optional>

namespace sf
{
	class RenderWindow;
	class Event;
}

class IState
{
public:
	virtual ~IState() = default;

	virtual void OnEnter() = 0;
	virtual void OnExit() = 0;
	virtual void Update(float dt) = 0;
	virtual void Render(sf::RenderWindow& window) = 0;
	virtual void HandleEvent(const sf::Event& event) = 0;

	virtual bool IsTransparent() const { return false; }

	// Colour the whole window (incl. fullscreen letterbox bars) is cleared to before this state draws.
	// nullopt -> the owner keeps its default clear colour.
	virtual std::optional<sf::Color> ClearColor() const { return std::nullopt; }
};
