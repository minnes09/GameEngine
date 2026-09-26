#pragma once

namespace sf
{
class RenderWindow;
}

template<typename T>
class ComponentPool;
struct TransformComponent;
struct RenderableComponent;

class RenderSystem
{
public:
	void Render(sf::RenderWindow& window);

private:
	void renderTexts(ComponentPool<TransformComponent>* transformPool,
	                 ComponentPool<RenderableComponent>* renderablePool,
	                 sf::RenderWindow& window);
	void renderShapes(ComponentPool<TransformComponent>* transformPool,
	                  ComponentPool<RenderableComponent>* renderablePool,
	                  sf::RenderWindow& window);
};
