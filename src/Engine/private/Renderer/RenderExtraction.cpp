#include "Renderer/RenderExtraction.h"

#include "ECS/ComponentPool.h"
#include "ECS/Components/Renderable.h"
#include "ECS/Components/Shape.h"
#include "ECS/Components/Text.h"
#include "ECS/Components/Transform.h"
#include "ECS/EntityManager.h"
#include "Renderer/RenderQueue.h"

namespace RenderExtraction_Private
{
namespace
{
ShapeKind ToShapeKind(Shape shape)
{
	switch (shape)
	{
		case Shape::Circle:
			return ShapeKind::Circle;
		case Shape::Rectangle:
			break;
	}
	return ShapeKind::Rectangle;
}
} // namespace
} // namespace RenderExtraction_Private

void ExtractRenderQueue(const ComponentPool<TransformComponent>&  transforms,
                        const ComponentPool<RenderableComponent>& renderables,
                        const ComponentPool<TextComponent>*       texts,
                        RenderQueue&                              out)
{
	renderables.ForEach(
	    [&](DEPRECATED_EntityId entity, const RenderableComponent& renderable)
	    {
		    const auto* transform = transforms.Get(entity);
		    if (!transform)
			    return;

		    out.PushShape({ .kind             = RenderExtraction_Private::ToShapeKind(renderable.shape),
		                    .position         = transform->position,
		                    .size             = renderable.size,
		                    .fillColor        = renderable.color,
		                    .outlineColor     = renderable.outlineColor,
		                    .outlineThickness = renderable.outlineThickness,
		                    .textureId        = renderable.textureId });
	    });

	if (!texts)
		return;

	texts->ForEach(
	    [&](DEPRECATED_EntityId entity, const TextComponent& text)
	    {
		    const auto* transform = transforms.Get(entity);
		    if (!transform)
			    return;

		    RenderVec2 center = transform->position;
		    if (const auto* renderable = renderables.Get(entity))
			    // size is the full box for both shapes (a circle is inscribed in it), so its half is the center.
			    center += renderable->size * 0.5f;

		    out.PushText({ .center        = center,
		                   .text          = text.text,
		                   .characterSize = text.characterSize,
		                   .color         = text.color });
	    });
}

void ExtractRenderQueue(EntityManager& entities, RenderQueue& out)
{
	const auto* transforms = entities.GetPool<TransformComponent>();
	if (!transforms)
		return;

	// No Renderable pool yet (e.g. a text-only scene): extract texts against an empty pool.
	static const ComponentPool<RenderableComponent> kNoRenderables;
	const auto* renderables = entities.GetPool<RenderableComponent>();
	ExtractRenderQueue(*transforms, renderables ? *renderables : kNoRenderables, entities.GetPool<TextComponent>(), out);
}
