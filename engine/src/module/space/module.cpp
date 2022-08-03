#include <cmath>
#include <engine/Engine.hpp>
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
  world.component<internal::BBoxSize>("internal::BBoxSize")
    .member<decltype(internal::BBoxSize::x)>("x")
    .member<decltype(internal::BBoxSize::y)>("y");

  //init singleton
  world.set<internal::WorldSpatialIndex>({
    .p_index = internal::SpatialIndex::create_index(),
    .q_for_init = world.query_builder<const Position, const internal::BBoxSize>()
      .term<internal::UpdateTreeObject>()
      .term<internal::WorldTreeObject>().oper(flecs::Not).read_write()
      .build(),
    .q_for_update = world.query_builder<const Position, const internal::BBoxSize, internal::WorldTreeObject>()
      .term<internal::UpdateTreeObject>()
      .build()
  });

  world.observer<const Object>("observers::UpdateBBox_Object")
    .event(flecs::OnAdd)
    .event(flecs::OnSet)
    .term<Scale>().oper(flecs::Not)
    .term<Rotation>().oper(flecs::Not)
    .each(internal::Observer_UpdateBBox_Object);
  
  world.observer<const Object, const Scale>("observers::UpdateBBox_ObjectScale")
    .event(flecs::OnAdd)
    .event(flecs::OnSet)
    .term<Rotation>().oper(flecs::Not)
    .each(internal::Observer_UpdateBBox_ObjectScale);
  
  world.observer<const Object, const Rotation>("observers::UpdateBBox_ObjectRotation")
    .event(flecs::OnAdd)
    .event(flecs::OnSet)
    .term<Scale>().oper(flecs::Not)
    .each(internal::Observer_UpdateBBox_ObjectRotation);
  
  world.observer<const Object, const Rotation, const Scale>("observers::UpdateBBox_ObjectRotationScale")
    .event(flecs::OnAdd)
    .event(flecs::OnSet)
    .each(internal::Observer_UpdateBBox_ObjectRotationScale);

  world.observer("observers::RemoveBBox")
    .event(flecs::OnRemove)
    .term<Object>()
    .each(internal::Observer_RemoveBBox);

  world.observer("observers::UpdatePosition")
    .event(flecs::OnAdd)
    .event(flecs::OnSet)
    .term<Object>()
    .term<Position>()
    .each([](flecs::entity e) {
      e.add<internal::UpdateTreeObject>();
    });

  world.system<internal::WorldSpatialIndex>("systems::SpatialIndexUpdate")
    .kind<MainThread_Pre>()
    .arg(1).read_write().singleton()
    .iter(internal::System_SpatialIndexUpdate);

  world.system<const internal::WorldTreeObject>("systems::HandeIntersections")
    .kind(flecs::OnUpdate)
    .term<internal::WorldSpatialIndex>().singleton()
    .term<internal::UpdateTreeObject>()
    .term<HandleIntersections>()
    .iter(internal::System_HandeIntersections);
};



};