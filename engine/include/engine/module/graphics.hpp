#pragma once
#include <cstdint>
#include <flecs.h>

namespace engine {

struct Graphics {
  Graphics(flecs::world&);
};

} //namespace engine

namespace engine::graphics {

struct ColoredSquare {};

struct Layer {
  float layer;
};

struct Color {
  std::uint8_t r;
  std::uint8_t g;
  std::uint8_t b;

  static const Color Black;
  static const Color White;
  static const Color Red;     
  static const Color Green;   
  static const Color Blue;   
  static const Color Yellow;  
  static const Color Magenta; 
  static const Color Cyan;
};

struct Alpha {
  std::uint8_t a;
};

struct BlendAdd {};
struct BlendSubstract {};
struct BlendMultiply {};

struct Camera {};

} //namespace engine


#include "graphics/graphics.inl"