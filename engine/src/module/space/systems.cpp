#include <spdlog/spdlog.h>
#include <cassert>
#include <engine/module/space.hpp>
#include "systems.hpp"
#include "internal_components.hpp"

namespace engine::space::internal {

void Observer_OnAdd_Object_Position(flecs::entity e, const Object&, const Position&) {
  e.add<internal::UpdateTreeObject>();
}

void System_InitTreeObject(flecs::iter it, const Object* p_objects, const Position* p_positions) {
  for (auto i : it) {
    SPDLOG_WARN("{}", it.entity(i).name());
  }
}

void System_SpatialIndexUpdate(flecs::iter it, WorldSpatialIndex* p_world_index) {
  p_world_index->q_for_init.iter([p_world_index](flecs::iter it) {
    for (auto i : it) {
      it.entity(i).set<WorldTreeObject>({.p_object=p_world_index->p_index->create_object()});
    }
  });
  //p_world_index->p_index->process_queues();
}


}