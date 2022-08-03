#pragma once
#include <glm/vec2.hpp>
#include <glm/trigonometric.hpp>
#include <glm/common.hpp>

namespace engine::space::internal {

//Описывает неповернутый прямоугольник вокруг повернутого прямоугольника
inline glm::vec2 describe_rectangle(glm::vec2 size, float angle) {
  float abs_sin_angle = glm::abs(glm::sin(angle));
  float abs_cos_angle = glm::abs(glm::cos(angle));
  return {
    size.x * abs_cos_angle + size.y * abs_sin_angle,
    size.x * abs_sin_angle + size.y * abs_cos_angle
  };
}

};