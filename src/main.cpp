#include "日志.hpp"
#define SDL_MAIN_USE_CALLBACKS 1 /* 使用回调函数而不是main() */
#include "SDL3/SDL_init.h"
#include "应用.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <Windows.h>

// 设置控制台编码
void 设置控制台编码_utf8() {
  SetConsoleOutputCP(CP_UTF8);
  SetConsoleCP(CP_UTF8);
}

/* 此函数在启动时运行一次。 */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  设置控制台编码_utf8();
  spdlog::set_level(spdlog::level::debug);
  if (!引擎::核心::应用::初始化()) {
    记录错误("初始化失败！");
  }

  return SDL_APP_CONTINUE; /* 继续执行程序！ */
}

/* 此函数在发生新事件（鼠标输入、按键等）时运行。 */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {

  引擎::核心::应用::获取实例().处理事件(*event);

  return SDL_APP_CONTINUE; /* 继续执行程序！ */
}

/* 此函数在每一帧运行一次，是程序的核心。 */
SDL_AppResult SDL_AppIterate(void *appstate) {
  if (引擎::核心::应用::获取实例().是否已退出()) {
    return SDL_APP_SUCCESS; /* 结束程序，向操作系统报告成功。 */
  }

  引擎::核心::应用::获取实例().运行();

  return SDL_APP_CONTINUE; /* 继续执行程序！ */
}

/* 此函数在关闭时运行一次。 */
void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  引擎::核心::应用::销毁实例();
}
