#pragma once

namespace engine::graphics {

inline constexpr Color Color::Black = {0, 0, 0};
inline constexpr Color Color::White = {255, 255, 255};
inline constexpr Color Color::Red = {255, 0, 0};
inline constexpr Color Color::Green = {0, 255, 0};
inline constexpr Color Color::Blue = {0, 0, 255};
inline constexpr Color Color::Yellow = {255, 255, 0};
inline constexpr Color Color::Magenta = {255, 0, 255};
inline constexpr Color Color::Cyan = {0, 255, 255};

constexpr bool operator==(const Color& left, const Color& right) {
  return (left.r == right.r) && (left.g == right.g) && (left.b == right.b);
}

constexpr bool operator!=(const Color& left, const Color& right) {
    return !(left == right);
}

} //namespace engine
