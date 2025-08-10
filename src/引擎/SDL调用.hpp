#pragma once

#include "SDL3/SDL.h"
#include "日志.hpp"

#define SDL调用(expr)                                                          \
  do {                                                                         \
    if (!(expr)) {                                                             \
      记录错误("SDL错误: {}", SDL_GetError());                                 \
    }                                                                          \
  } while (0)