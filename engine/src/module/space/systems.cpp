#include <spdlog/spdlog.h>
#include <engine/module/space.hpp>
#include "systems.hpp"
#include "internal_components.hpp"
#include "rect_math.hpp"

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



//BBOX CALCS
void Observer_RemoveBBox(flecs::entity e) {
  e.remove<BBoxSize>();
}

void Observer_UpdateBBox_ObjectRotationScale(flecs::entity e, const Object& obj, const Rotation& rotation, const Scale& scale) {
  if (obj.x <= 0 || obj.y <= 0 || scale.x == 0 || scale.y == 0) {
    e.remove<BBoxSize>();
    return;
  }
  glm::vec2 bbox_size = describe_rectangle({obj.x*scale.x, obj.y*scale.y}, rotation.angle);
  e.set<BBoxSize>({
    .x = bbox_size.x,
    .y = bbox_size.y
  });  
}

void Observer_UpdateBBox_ObjectRotation(flecs::entity e, const Object& obj, const Rotation& rotation) {
  if (obj.x <= 0 || obj.y <= 0) {
    e.remove<BBoxSize>();
    return;
  }
  glm::vec2 bbox_size = describe_rectangle({obj.x, obj.y}, rotation.angle);
  e.set<BBoxSize>({
    .x = bbox_size.x,
    .y = bbox_size.y
  });  
}

void Observer_UpdateBBox_ObjectScale(flecs::entity e, const Object& obj, const Scale& scale) {
  if (obj.x <= 0 || obj.y <= 0 || scale.x == 0 || scale.y == 0) {
    e.remove<BBoxSize>();
    return;
  }
  float abs_scale_x = std::abs(scale.x);
  float abs_scale_y = std::abs(scale.y);
  e.set<BBoxSize>({
    .x = obj.x * glm::abs(scale.x),
    .y = obj.y * glm::abs(scale.y)
  });  
}

void Observer_UpdateBBox_Object(flecs::entity e, const Object& obj) {
  if (obj.x <= 0 || obj.y <= 0) {
    e.remove<BBoxSize>();
    return;
  }
  e.set<BBoxSize>({
    .x = obj.x,
    .y = obj.y
  });
}

} //namespace engine::space::internal