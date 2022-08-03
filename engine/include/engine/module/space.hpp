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
struct Position {
  float x;
  float y;
};
struct Rotation {
  float angle; //in radians
};
struct Scale {
  float x;
  float y;
};

struct HandleIntersections {};
struct IntersectsWith {}; //relation

} //engine::space namespace