#pragma once
#include <flecs.h>
#include <engine/module/space.hpp>
#include "internal_components.hpp"

namespace engine::space::internal {

void Observer_OnAdd_Object_Position(flecs::entity, const Object&, const Position&);

void System_InitTreeObject(flecs::iter, const Object*, const Position*);

void System_SpatialIndexUpdate(flecs::iter, WorldSpatialIndex*);
  
}