#pragma once

#include "SDL3/SDL.h"
#include "glm/ext/vector_int2.hpp"
#include <string>

namespace 引擎::核心 {

class 窗口 {
public:
  窗口(const std::string &标题, int 宽度, int 高度, Uint32 flags = 0);
  窗口(const 窗口 &) = delete; // 禁止复制构造
  窗口 &operator=(const 窗口 &) = delete;
  ~窗口();

  glm::ivec2 获取窗口宽高() const;

  SDL_Window *获取窗口();

private:
  SDL_Window *_窗口{};
};
} // namespace 引擎::核心