#include <engine/module/space.hpp>
#include "systems.hpp"
#include "internal_components.hpp"

namespace engine::space::internal {

void Observer_OnAdd_Object_Position(flecs::entity e, const Object&, const Position&) {
  e.add<internal::UpdateTreeObject>();
}

}