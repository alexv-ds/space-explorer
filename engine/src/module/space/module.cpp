#include <engine/module/space.hpp>
#include "internal_components.hpp"
#include "systems.hpp"

#include <spdlog/spdlog.h>

namespace engine {
using namespace space;

Space::Space(flecs::world& world) {
  world.module<Space>("space");

  world.component<Object>()
    .member<decltype(Object::x)>("x")
    .member<decltype(Object::y)>("y");
  world.component<Position>()
    .member<decltype(Position::x)>("x")
    .member<decltype(Position::y)>("y");
  world.component<Rotation>()
    .member<decltype(Rotation::angle)>("angle");
  world.component<Scale>()
    .member<decltype(Scale::x)>("x")
    .member<decltype(Scale::y)>("y");
  world.component<HandleIntersections>();
  world.component<IntersectsWith>()
    .add(flecs::Symmetric);
  world.component<internal::WorldSpatialIndex>("internal::WorldSpatialIndex");
  world.component<internal::WorldTreeObject>("internal::WorldTreeObject");
  world.component<internal::UpdateTreeObject>("internal::UpdateTreeOpject");
  world.component<internal::LoadIntersections>("internal::LoadIntersections");
  world.component<internal::BBoxSize>("internal::BBoxSize")
    .member<decltype(internal::BBoxSize::x)>("x")
    .member<decltype(internal::BBoxSize::y)>("y");

  //init singleton
  world.set<internal::WorldSpatialIndex>({.p_index = internal::SpatialIndex::create_index()});

  world.observer<const Object, const Position>("observers::OnAdd_Object_Position")
    .term<internal::WorldSpatialIndex>().singleton()
    .event(flecs::OnAdd)
    .each(internal::Observer_OnAdd_Object_Position);





  


};



};