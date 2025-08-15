#pragma once

#include "SDL3/SDL.h"
#include "窗口.hpp"

namespace 引擎::核心 {

class 图像;
class 颜色;

class 渲染器 {
public:
  explicit 渲染器(窗口 &窗口引用); // explicit 防止隐式转换
  渲染器(const 渲染器 &) = delete;
  渲染器 &operator=(const 渲染器 &) = delete;
  渲染器(渲染器 &&) = delete;
  渲染器 &operator=(渲染器 &&) = delete;
  ~渲染器();

  void 设置混合模式(SDL_BlendMode 模式 = SDL_BLENDMODE_BLEND);
  void 设置Vsync(bool 启用 = true);
  void 清屏(const 颜色 &颜色);
  void 设置缩放(float x, float y);
  void 渲染呈现();

  // void DrawLine(const glm::vec2 &p1, const glm::vec2 &p2, const 颜色 &颜色);
  // void DrawRect(const Rect &, const 颜色 &);
  // void DrawCircle(const Circle &, const 颜色 &, uint32_t fragment = 20);
  // void FillRect(const Rect &, const 颜色 &);
  // void DrawImage(const 图像 &, const Region &src, const Region &dst,
  //                Degrees rotation, const glm::vec2 &center, Flags<Flip>);
  // void DrawTiled(const 图像 &, const Region &src, const Region &dst,
  //                float scale);

  SDL_Renderer *获取渲染器() const;

private:
  SDL_Renderer *_渲染器{};
};
} // namespace 引擎::核心