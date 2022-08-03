#include <engine/Engine.hpp>
#include <engine/module/space.hpp>
#include "internal_components.hpp"
#include "systems.hpp"

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
  world.set<internal::WorldSpatialIndex>({
    .p_index = internal::SpatialIndex::create_index(),
    .q_for_init = world.query_builder<>()
      .term<Position>()
      .term<internal::BBoxSize>()
      .term<internal::UpdateTreeObject>()
      .term<internal::WorldTreeObject>().oper(flecs::Not).read_write()
      .build(),
  });

  world.observer<const Object, const Position>("observers::OnAdd_Object_Position")
    .event(flecs::OnAdd)
    .each(internal::Observer_OnAdd_Object_Position);

  /*world.system<const Object, const Position>("systems::InitTreeObject")
    .kind<MainThread_Pre>()
    .term<internal::UpdateTreeObject>()
    .term<internal::WorldTreeObject>().oper(flecs::Not)
    .term<internal::WorldSpatialIndex>().singleton()
    .iter(internal::System_InitTreeObject);*/

  world.system<internal::WorldSpatialIndex>("systems::SpatialIndexUpdate")
    .kind<MainThread_Pre>()
    .arg(1).inout(flecs::InOut).singleton()
    .iter(internal::System_SpatialIndexUpdate);


  


};



};