#pragma once

#include "SDL3/SDL_events.h"
#include "SDL3/SDL_video.h"
#include "imgui_impl_sdl3.h"
#include <SDL3/SDL.h>
#include <memory>

class 应用 final {
private:
  static std::unique_ptr<应用> _应用实例;
  SDL_Window *_窗口;
  SDL_Renderer *_渲染器;
  bool _是否退出 = false;
  float _显示比例 = 1.0f; // 显示比例，用于缩放窗口

public:
  static void 初始化();
  static void 销毁实例();
  static 应用 &获取实例();

  应用();
  ~应用();

  应用(const 应用 &) = delete;
  应用 &operator=(const 应用 &) = delete;
  应用(应用 &&) = delete;
  应用 &operator=(应用 &&) = delete;

  void 运行();

  void 处理事件(SDL_Event &事件);

  void 更新迭代(float 帧间隔时长);

  void 绘制画面();

  bool 是否已退出();
};