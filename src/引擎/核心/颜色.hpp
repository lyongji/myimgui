#pragma once
#include "SDL3/SDL.h"
#include "glm/ext/vector_float3.hpp"
#include "glm/ext/vector_float4.hpp"
#include <format>
#include <string>

namespace 引擎::核心 {

struct 颜色 {
  float r = 0.0f, g = 0.0f, b = 0.0f, a = 1.0f; // 默认为黑色，不透明
  颜色() = default;
  explicit 颜色(glm::vec3 颜色值) : 颜色(颜色值.r, 颜色值.g, 颜色值.b, 1.0f) {}
  explicit 颜色(glm::vec4 颜色值)
      : 颜色(颜色值.r, 颜色值.g, 颜色值.b, 颜色值.a) {}
  颜色(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}
  operator glm::vec4() const { return {r, g, b, a}; }
  operator glm::vec3() const { return {r, g, b}; }
  operator SDL_Color() const {
    return {static_cast<Uint8>(r * 255), static_cast<Uint8>(g * 255),
            static_cast<Uint8>(b * 255), static_cast<Uint8>(a * 255)};
  }
  // 格式化输出
  std::string toString() const {
    return std::format("颜色 (r={:.2f}, g={:.2f}, b={:.2f},a={:.2f})", r, g, b,
                       a);
  }
};

} // namespace 引擎::核心