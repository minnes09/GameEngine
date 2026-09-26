#include "ECS/ComponentType.h"

#include <array>
#include <atomic>

namespace ComponentType_Private
{
namespace
{
// Constant-initialised: safe to use from static initialisers of other translation units.
std::array<ComponentMeta, kMaxComponentTypes> metas{};
std::atomic<std::uint32_t>                    registeredCount{ 0 };
} // namespace
} // namespace ComponentType_Private

ComponentId ComponentTypeRegistry::Register(const ComponentMeta& meta)
{
	const std::uint32_t index = ComponentType_Private::registeredCount.fetch_add(1, std::memory_order_relaxed);
	MYRIAS_ASSERT(index < kMaxComponentTypes, "too many component types: raise kMaxComponentTypes");
	// Published to readers by the thread-safe static initialisation in GetComponentId<T>(): whoever
	// obtains the id also sees the entry.
	ComponentType_Private::metas[index] = meta;
	return static_cast<ComponentId>(index);
}

const ComponentMeta& ComponentTypeRegistry::Get(ComponentId id) noexcept
{
	MYRIAS_ASSERT(id < ComponentType_Private::registeredCount.load(std::memory_order_relaxed),
	              "unknown ComponentId");
	return ComponentType_Private::metas[id];
}

std::size_t ComponentTypeRegistry::Count() noexcept
{
	return ComponentType_Private::registeredCount.load(std::memory_order_relaxed);
}
