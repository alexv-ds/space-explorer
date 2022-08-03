#pragma once
#include <glm/vec2.hpp>
#include "SpatialIndex.hpp"
#include "TreeObject.hpp"

namespace engine::space::internal {

struct WorldTreeObject {
  std::shared_ptr<TreeObject> p_object;
};

struct UpdateTreeObject {};
struct BBoxSize {
  float x;
  float y;
};

struct WorldSpatialIndex {
  std::shared_ptr<SpatialIndex> p_index;
  flecs::query<const Position, const BBoxSize> q_for_init;
  flecs::query<const Position, const BBoxSize, WorldTreeObject> q_for_update;
};


}