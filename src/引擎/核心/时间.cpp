#include "时间.hpp"
#include "SDL3/SDL_Timer.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_timer.h"
#include "日志.hpp"
#include <cstdint>

时间::时间() {
  // 初始化为当前时间点,防止第一帧时间差过大
  _上帧时间点 = SDL_GetTicksNS();
  _此帧开始时间点 = _上帧时间点;
  记录跟踪("时间初始化，上帧时间点：{}", _上帧时间点);
}

void 时间::更新() {
  _此帧开始时间点 = SDL_GetTicksNS(); // 记录进入此帧的时间点
  auto 耗时 = (_此帧开始时间点 - _上帧时间点) / 1.0e9;
  // 如果设置了目标帧率,则进行帧率控制;否则 _帧间时长 = 耗时
  if (_目标每帧时长 > 0.0) {
    限制帧率(耗时);
  } else {
    _帧间时长 = 耗时;
  }
  // 记录离开 更新() 的时间点
  auto _上帧时间点 = SDL_GetTicksNS(); // 获取当前时间
}

void 时间::限制帧率(float 当前帧已执行时长) {
  // 如果当前帧耗时小于目标帧间隔,则等待剩余时间
  if (当前帧已执行时长 < _目标每帧时长) {
    auto 剩余等待时长 = _目标每帧时长 - 当前帧已执行时长;
    auto 剩余等待时长NS = static_cast<Uint64>(剩余等待时长 * 1.0e9);
    SDL_DelayNS(剩余等待时长NS);
    _帧间时长 = static_cast<double>(SDL_GetTicksNS() - _上帧时间点) / 1.0e9;
  }
}