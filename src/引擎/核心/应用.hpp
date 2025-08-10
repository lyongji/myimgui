#pragma once

#include "SDL3/SDL_events.h"
#include "SDL3/SDL_video.h"
#include "imgui_impl_sdl3.h"
#include "时间.hpp"
#include <SDL3/SDL.h>
#include <memory>

// 前向声明, 减少头文件的依赖，增加编译速度

namespace 引擎::核心 {
class 时间;
}

namespace 引擎::核心 {
class 应用 final {
private:
  static std::unique_ptr<应用> _应用实例;
  SDL_Window *_窗口;
  SDL_Renderer *_渲染器;
  bool _是否退出 = false;
  float _显示比例 = 1.0f; // 显示比例，用于缩放窗口

  // 各模块的指针，在初始化()中创建
  std::unique_ptr<引擎::核心::时间> _时间;

public:
  [[nodiscard]] static bool 初始化();
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

private:
  // 各模块的初始化/创建函数，在初始化()中调用
  [[nodiscard]] bool 初始化SDL();
  [[nodiscard]] bool 初始化ImGui();
  [[nodiscard]] bool 初始化时间();
};
} // namespace 引擎::核心