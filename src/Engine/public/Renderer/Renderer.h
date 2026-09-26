#pragma once

#include "Renderer/RenderQueue.h"
#include "Renderer/RenderTypes.h"

#include <memory>

namespace sf
{
class RenderTarget;
}
class AssetManager;

// Draws a RenderQueue on a render target. The backend (SFML today) lives entirely in
// private/Renderer/Sfml: this header exposes no backend drawing types.
//
// Per frame: BeginFrame() -> fill Queue() -> EndFrame(). Presenting the frame (display()) and
// events stay with whoever owns the window.
class Renderer
{
public:
	// Owns neither: both must outlive the Renderer.
	Renderer(sf::RenderTarget& target, const AssetManager& assets);
	~Renderer() noexcept;
	Renderer(const Renderer&)            = delete;
	Renderer& operator=(const Renderer&) = delete;

	// Clears the queue and the target.
	void BeginFrame(RenderColor clearColor);

	[[nodiscard]] RenderQueue& Queue() noexcept { return queue; }

	// Draws the queued shapes, then the texts on top.
	void EndFrame();

private:
	struct Backend;

	sf::RenderTarget&        target;
	const AssetManager&      assets;
	RenderQueue              queue;
	std::unique_ptr<Backend> backend;
};
