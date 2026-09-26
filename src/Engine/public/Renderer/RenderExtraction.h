#pragma once

// Pure extraction: reads the ECS render components and appends draw commands to a RenderQueue.
// No drawing, no GPU, no window: testable with plain component pools.

template <typename T>
class ComponentPool;
struct TransformComponent;
struct RenderableComponent;
struct TextComponent;
class EntityManager;
class RenderQueue;

// Appends to `out` (does not clear it). `texts` may be null. Entities without a Transform are skipped.
// Text is centered in the entity's Renderable box when it has one, otherwise on its position.
void ExtractRenderQueue(const ComponentPool<TransformComponent>&  transforms,
                        const ComponentPool<RenderableComponent>& renderables,
                        const ComponentPool<TextComponent>*       texts,
                        RenderQueue&                              out);

// Convenience overload: takes the pools from `entities`. Does nothing if the Transform pool does
// not exist; a missing Renderable or Text pool just means nothing of that kind to draw.
void ExtractRenderQueue(EntityManager& entities, RenderQueue& out);
