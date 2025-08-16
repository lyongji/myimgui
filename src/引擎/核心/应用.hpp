#pragma once

#include "SDL3/SDL_events.h"
#include "SDL3/SDL_video.h"
#include "imgui_impl_sdl3.h"
#include <SDL3/SDL.h>
#include <memory>

// 前向声明, 减少头文件的依赖，增加编译速度
namespace 引擎::资源 {
class 资源管理器;
}

namespace 引擎::核心 {
class 时间;
class 窗口;

} // namespace 引擎::核心
namespace 引擎::渲染 {
class 渲染器;

}

namespace 引擎::核心 {
class 应用 final {
private:
  static std::unique_ptr<应用> _应用实例;

  bool _是否退出 = false;
  float _显示比例 = 1.0f; // 显示比例，用于缩放窗口
                          //
  // 各模块的指针，在初始化()中创建
  std::unique_ptr<引擎::核心::时间> _时间;
  std::unique_ptr<引擎::核心::窗口> _窗口;
  std::unique_ptr<引擎::渲染::渲染器> _渲染器;
  std::unique_ptr<引擎::资源::资源管理器> _资源管理器;

public:
  [[nodiscard]] static bool 初始化();
  // 获取单例实例的静态方法
  static 应用 &获取实例();
  static void 销毁实例();

  ~应用();

  应用(const 应用 &) = delete;
  应用 &operator=(const 应用 &) = delete;
  应用(应用 &&) = delete;
  应用 &operator=(应用 &&) = delete;

  void 运行();

  void 处理事件(SDL_Event &事件);

  void 更新逻辑(float 帧间隔时长);
  void 更新UI();

  void 绘制画面();

  bool 是否已退出();

private:
  应用(); // 私有构造函数，防止外部创建实例
  // 各模块的初始化/创建函数，在初始化()中调用
  [[nodiscard]] bool 执行组件初始化();
  [[nodiscard]] bool 初始化SDL();
  [[nodiscard]] bool 初始化ImGui();
  [[nodiscard]] bool 初始化时间();
  [[nodiscard]] bool 初始化资源管理器();
  void 关闭组件();
  void 绘制UI();
  void 测试资源管理器();
  void 测试纹理();
  void 测试音频();
  void 测试字体();
};
} // namespace 引擎::核心