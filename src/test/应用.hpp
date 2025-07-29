#pragma once

#include "SDL3/SDL_events.h"
#include "SDL3/SDL_video.h"
#include "imgui_impl_sdl3.h"
#include <SDL3/SDL.h>
#include <memory>

class 应用 { // 定义一个名为“应用”的类
public:
  static void 初始化();   // 静态成员函数，用于初始化应用
  static void 销毁实例(); // 静态成员函数，用于清理应用
  static 应用 &获取实例();

  应用();
  应用(const 应用 &) = delete;
  应用 &operator=(const 应用 &) = delete;
  应用(应用 &&) = delete;
  应用 &operator=(应用 &&) = delete;
  ~应用();

  // 静态成员函数实例化，用于获取应用类的唯一实例（单例模式）

  void 处理事件(SDL_Event &事件); // 处理SDL事件
  // 成员函数运行，用于启动应用
  void 刷新显示();

  bool 是否已退出();

private:
  static std::unique_ptr<应用> _应用实例;
  SDL_Window *_窗口;
  SDL_Renderer *_渲染器;
  bool _是否退出 = false;
  float _显示比例 = 1.0f;   // 显示比例，用于缩放窗口
  Uint64 _帧率值 = 60;      // 期望帧率（帧/秒）
  Uint64 _帧延迟纳秒值 = 0; // 纳秒，基于帧率的等待时间
  float _帧间隔时长 = 0.0f; // 实际帧间隔（秒）
};