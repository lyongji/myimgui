#pragma once

#include "渲染器.hpp"
#include "SDL3/SDL.h"
#include "SDL调用.hpp"
#include "日志.hpp"
#include "窗口.hpp"
#include "颜色.hpp"

namespace 引擎::核心 {

渲染器::渲染器(窗口 &窗口引用) {
  _渲染器 = SDL_CreateRenderer(窗口引用.获取窗口(), nullptr);
  if (!_渲染器) {
    记录错误("创建 SDL 渲染器失败: {}", SDL_GetError());
  }
  SDL调用(SDL_SetRenderDrawBlendMode(_渲染器, SDL_BLENDMODE_BLEND));
}

渲染器::~渲染器() { SDL_DestroyRenderer(_渲染器); }

void 渲染器::设置清除颜色(const 颜色 &目标颜色) { _清除颜色 = 目标颜色; }

// void DrawLine(const glm::vec2 &p1, const glm::vec2 &p2, const 颜色 &颜色);
// void DrawRect(const Rect &, const 颜色 &);
// void DrawCircle(const Circle &, const 颜色 &, uint32_t fragment = 20);
// void FillRect(const Rect &, const 颜色 &);
// void DrawImage(const 图像 &, const Region &src, const Region &dst,
//                Degrees rotation, const glm::vec2 &center, Flags<Flip>);
// void DrawTiled(const 图像 &, const Region &src, const Region &dst,
//                float scale);

void 渲染器::清除画面() {
  SDL调用(SDL_SetRenderDrawColor(_渲染器, _清除颜色.r, _清除颜色.g, _清除颜色.b,
                                 _清除颜色.a));
  SDL调用(SDL_RenderClear(_渲染器));
}
void 渲染器::提交渲染() { SDL调用(SDL_RenderPresent(_渲染器)); }

SDL_Renderer *渲染器::获取渲染器() const { return _渲染器; }

void 渲染器::设置渲染颜色(const 颜色 &渲染颜色) {
  SDL调用(SDL_SetRenderDrawColor(_渲染器, 渲染颜色.r() * 255,
                                 渲染颜色.g() * 255, 渲染颜色.b() * 255,
                                 渲染颜色.a() * 255));
}
void 渲染器::设置混合模式(SDL_BlendMode 模式) {
  SDL_SetRenderDrawBlendMode(_渲染器, 模式);
}
} // namespace 引擎::核心