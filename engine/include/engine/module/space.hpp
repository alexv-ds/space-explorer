#pragma once
#include <flecs.h>
#include <glm/vec2.hpp>

namespace engine {

struct Space {
  Space(flecs::world&);
};

}; //engine namespace

namespace engine::space {

struct Object : glm::vec2 {};
struct Position : glm::vec2 {};
struct Rotation {
  float angle;
};
struct Scale : glm::vec2 {};
struct HandleIntersections {};
struct IntersectsWith {}; //relation

} //engine::space namespace