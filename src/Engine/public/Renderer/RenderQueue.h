#pragma once

#include "Renderer/RenderTypes.h"

#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <string_view>
#include <vector>

enum class ShapeKind : std::uint8_t
{
	Rectangle,
	Circle
};

struct ShapeDrawCommand
{
	ShapeKind   kind = ShapeKind::Rectangle;
	RenderVec2  position{};
	RenderVec2  size{}; // full box; a circle is inscribed in it (radius = size.x * 0.5)
	RenderColor fillColor{};
	RenderColor outlineColor{};
	float       outlineThickness = 0.f;
	// If the backend resolves it, the command draws as a sprite scaled to `size` and tinted by
	// `fillColor`; otherwise it falls back to the shape. Must point to storage that outlives the frame.
	const char* textureId = nullptr;
};

struct TextDrawCommand
{
	RenderVec2       center{};
	std::string_view text; // PushText copies it: the source only needs to live for the call
	unsigned int     characterSize = 16;
	RenderColor      color{};
};

// Per-frame list of draw commands, filled by extraction (or by the game) and consumed by the Renderer.
// Text is copied into a queue-owned buffer, so callers may push temporaries.
// Clear() keeps every capacity, so after warm-up a frame does not allocate.
class RenderQueue
{
public:
	void Reserve(std::size_t shapeCount, std::size_t textCount, std::size_t textBytes = 0);
	void Clear() noexcept;

	void PushShape(const ShapeDrawCommand& command);
	void PushText(const TextDrawCommand& command);

	[[nodiscard]] std::span<const ShapeDrawCommand> Shapes() const noexcept { return shapes; }

	[[nodiscard]] std::size_t TextCount() const noexcept { return texts.size(); }
	// The returned view points into the queue's buffer: valid until the next PushText or Clear.
	[[nodiscard]] TextDrawCommand Text(std::size_t index) const noexcept;

	[[nodiscard]] std::size_t ShapeCapacity() const noexcept { return shapes.capacity(); }
	[[nodiscard]] std::size_t TextCapacity() const noexcept { return texts.capacity(); }
	[[nodiscard]] std::size_t TextBufferCapacity() const noexcept { return textBuffer.capacity(); }

private:
	// Offsets instead of views: growing the buffer would invalidate views.
	struct StoredText
	{
		RenderVec2    center{};
		std::uint32_t offset = 0;
		std::uint32_t length = 0;
		unsigned int  characterSize = 16;
		RenderColor   color{};
	};

	std::vector<ShapeDrawCommand> shapes;
	std::vector<StoredText>       texts;
	std::string                   textBuffer;
};
