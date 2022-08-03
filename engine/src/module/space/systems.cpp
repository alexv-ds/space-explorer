#include <cassert>
#include <vector>
#include <spdlog/spdlog.h>
#include <engine/module/space.hpp>
#include "systems.hpp"
#include "internal_components.hpp"
#include "rect_math.hpp"

namespace engine::space::internal {

void System_SpatialIndexUpdate(flecs::iter it, WorldSpatialIndex* p_world_index) {
  p_world_index->q_for_init.iter([p_world_index](flecs::iter it, const Position* p_pos, const BBoxSize* p_bbox) {
    for (auto i : it) {
      flecs::entity e = it.entity(i);
      std::shared_ptr<TreeObject> p_tree_object = p_world_index->p_index->create_object();
      p_tree_object->holder = e;
      p_tree_object->position.x = p_pos[i].x;
      p_tree_object->position.y = p_pos[i].y;
      p_tree_object->size.x = p_bbox[i].x;
      p_tree_object->size.y = p_bbox[i].y;
      e.set<WorldTreeObject>({.p_object=std::move(p_tree_object)});
    }
  });

  p_world_index->q_for_update.iter([](flecs::iter it, const Position* p_pos, const BBoxSize* p_bbox, WorldTreeObject* p_obj) {
    for (auto i : it) {
      p_obj->p_object->position.x = p_pos[i].x;
      p_obj->p_object->position.y = p_pos[i].y;
      p_obj->p_object->size.x = p_bbox[i].x;
      p_obj->p_object->size.y = p_bbox[i].y;
      p_obj->p_object->update();
    }
  });

  p_world_index->p_index->process_queues();
}

void System_HandeIntersections(flecs::iter it, const WorldTreeObject* p_tree_object) {
  flecs::world world = it.world();
  const WorldSpatialIndex* p_world_index = world.get<WorldSpatialIndex>();
  assert(p_world_index != nullptr);
  for (auto i : it) {
    flecs::entity this_entity = it.entity(i);
    this_entity.remove<internal::UpdateTreeObject>();
    this_entity.remove<IntersectsWith>(flecs::Wildcard);
    thread_local std::vector<flecs::entity_t> buffer;
    buffer.clear();
    p_world_index->p_index->intersects_query(buffer, p_tree_object->p_object->position, p_tree_object->p_object->size);
    
    for (flecs::entity_t raw_entity : buffer) {
      if(!world.is_alive(raw_entity) || raw_entity == this_entity) {
        continue;
      }
      flecs::entity entity = world.entity(raw_entity);
      this_entity.add<IntersectsWith>(entity);
    }
  }
}



//BBOX CALCS
void Observer_RemoveBBox(flecs::entity e) {
  e.remove<BBoxSize>();
  e.remove<UpdateTreeObject>();
  e.remove<IntersectsWith>(flecs::Wildcard);
}

void Observer_UpdateBBox_ObjectRotationScale(flecs::entity e, const Object& obj, const Rotation& rotation, const Scale& scale) {
  if (obj.x <= 0 || obj.y <= 0 || scale.x == 0 || scale.y == 0) {
    e.remove<BBoxSize>();
    e.remove<UpdateTreeObject>();
    e.remove<IntersectsWith>(flecs::Wildcard);
    return;
  }
  glm::vec2 bbox_size = describe_rectangle({obj.x*scale.x, obj.y*scale.y}, rotation.angle);
  e.set<BBoxSize>({
    .x = bbox_size.x,
    .y = bbox_size.y
  });
  e.add<UpdateTreeObject>();
}

void Observer_UpdateBBox_ObjectRotation(flecs::entity e, const Object& obj, const Rotation& rotation) {
  if (obj.x <= 0 || obj.y <= 0) {
    e.remove<BBoxSize>();
    e.remove<UpdateTreeObject>();
    e.remove<IntersectsWith>(flecs::Wildcard);
    return;
  }
  glm::vec2 bbox_size = describe_rectangle({obj.x, obj.y}, rotation.angle);
  e.set<BBoxSize>({
    .x = bbox_size.x,
    .y = bbox_size.y
  });
  e.add<UpdateTreeObject>();
}

void Observer_UpdateBBox_ObjectScale(flecs::entity e, const Object& obj, const Scale& scale) {
  if (obj.x <= 0 || obj.y <= 0 || scale.x == 0 || scale.y == 0) {
    e.remove<BBoxSize>();
    e.remove<UpdateTreeObject>();
    e.remove<IntersectsWith>(flecs::Wildcard);
    return;
  }
  e.set<BBoxSize>({
    .x = obj.x * glm::abs(scale.x),
    .y = obj.y * glm::abs(scale.y)
  });  
  e.add<UpdateTreeObject>();
}

void Observer_UpdateBBox_Object(flecs::entity e, const Object& obj) {
  if (obj.x <= 0 || obj.y <= 0) {
    e.remove<BBoxSize>();
    e.remove<UpdateTreeObject>();
    e.remove<IntersectsWith>(flecs::Wildcard);
    return;
  }
  e.set<BBoxSize>({
    .x = obj.x,
    .y = obj.y
  });
  e.add<UpdateTreeObject>();
}

} //namespace engine::space::internal