#include "Renderer/RenderQueue.h"

#include "Core/Assert.h"

#include <cstdint>
#include <limits>

void RenderQueue::Reserve(std::size_t shapeCount, std::size_t textCount, std::size_t textBytes)
{
	shapes.reserve(shapeCount);
	texts.reserve(textCount);
	textBuffer.reserve(textBytes);
}

void RenderQueue::Clear() noexcept
{
	shapes.clear();
	texts.clear();
	textBuffer.clear();
}

void RenderQueue::PushShape(const ShapeDrawCommand& command)
{
	shapes.push_back(command);
}

void RenderQueue::PushText(const TextDrawCommand& command)
{
	// Offsets and lengths are stored as 32 bits: a frame's text must stay under 4 GiB.
	MYRIAS_ASSERT(textBuffer.size() + command.text.size() <= std::numeric_limits<std::uint32_t>::max(),
	              "RenderQueue text buffer exceeds 4 GiB");
	const auto offset = static_cast<std::uint32_t>(textBuffer.size());
	textBuffer.append(command.text);
	texts.push_back({ .center        = command.center,
	                  .offset        = offset,
	                  .length        = static_cast<std::uint32_t>(command.text.size()),
	                  .characterSize = command.characterSize,
	                  .color         = command.color });
}

TextDrawCommand RenderQueue::Text(std::size_t index) const noexcept
{
	MYRIAS_ASSERT(index < texts.size(), "RenderQueue::Text index out of range");
	const StoredText& stored = texts[index];
	return { .center        = stored.center,
	         .text          = std::string_view(textBuffer).substr(stored.offset, stored.length),
	         .characterSize = stored.characterSize,
	         .color         = stored.color };
}
