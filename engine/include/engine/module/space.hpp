#pragma once
#include <flecs.h>
#include <glm/vec2.hpp>

namespace engine {

struct Space {
  Space(flecs::world&);
};

}; //engine namespace

namespace engine::space {

struct Object {
  float x;
  float y;
};
struct Position : glm::vec2 {};
struct Rotation {
  float angle;
};
struct Scale {
  float x;
  float y;
};

struct HandleIntersections {};
struct IntersectsWith {}; //relation

} //engine::space namespace