#include <engine/module/space.hpp>
#include "internal_components.hpp"
#include "TreeObject.hpp"
//#include "SpatialIndex.hpp"

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

  


};



};