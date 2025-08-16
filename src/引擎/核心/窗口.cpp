#include "窗口.hpp"
#include "SDL3/SDL.h"
#include "SDL调用.hpp"
#include "glm/ext/vector_int2.hpp"
#include "日志.hpp"
#include <string>


namespace 引擎::核心 {

窗口::窗口(const std::string &标题, int 宽度, int 高度, Uint32 flags) {
#ifdef SDL_PLATFORM_ANDROID
  _窗口 = SDL_CreateWindow(标题.c_str(), 0, 0, 0);
#else
  _窗口 = SDL_CreateWindow(标题.c_str(), 宽度, 高度, flags);
#endif
  if (!_窗口) {
    记录错误("窗口创建失败:{}", SDL_GetError());
  }
}

窗口::~窗口() { SDL_DestroyWindow(_窗口); }

glm::ivec2 窗口::获取窗口宽高() const {
  glm::ivec2 窗口宽高;
  SDL调用(SDL_GetWindowSize(_窗口, &窗口宽高.x, &窗口宽高.y));
  return 窗口宽高;
}

SDL_Window *窗口::获取窗口() { return _窗口; }
void 窗口::显示() { SDL_ShowWindow(_窗口); }
void 窗口::隐藏() { SDL_HideWindow(_窗口); }
void 窗口::最小化() { SDL_MinimizeWindow(_窗口); }
void 窗口::最大化() { SDL_MaximizeWindow(_窗口); }
void 窗口::恢复() { SDL_RestoreWindow(_窗口); }
void 窗口::设置全屏() { SDL_SetWindowFullscreen(_窗口, SDL_WINDOW_FULLSCREEN); }
void 窗口::居中() {
  SDL_SetWindowPosition(_窗口, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
}
} // namespace 引擎::核心