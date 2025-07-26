#define SDL_MAIN_USE_CALLBACKS 1 /* 使用回调函数而不是main() */
#include "SDL3/SDL_init.h"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

/* 我们将使用此渲染器在每一帧中绘制此窗口。 */
static SDL_Window *窗口 = nullptr;
static SDL_Renderer *渲染器 = nullptr;

/* 此函数在启动时运行一次。 */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  /* 初始化 SDL。 */
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_CAMERA | SDL_INIT_JOYSTICK |
                SDL_INIT_EVENTS)) {
    SDL_Log("无法初始化 SDL: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  if (!SDL_CreateWindowAndRenderer("窗口标题", 640, 480, 0, &窗口, &渲染器)) {
    SDL_Log("无法创建窗口/渲染器: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  // 设置Dear ImGui上下文
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // 启用键盘控制
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // 启用游戏手柄控制
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // 启用停靠

  // 设置Dear ImGui样式
  ImGui::StyleColorsDark();
  // ImGui::StyleColorsLight();

  // 设置平台/渲染器后端
  ImGui_ImplSDL3_InitForSDLRenderer(窗口, 渲染器);
  ImGui_ImplSDLRenderer3_Init(渲染器);

  auto 字体大小 = 20.0f;
  ImFont *字体 = io.Fonts->AddFontFromFileTTF(
      "./assets/MapleMono-NF-CN-Regular.ttf", 字体大小);
  IM_ASSERT(字体 != nullptr);

  return SDL_APP_CONTINUE; /* 继续执行程序！ */
}

/* 此函数在发生新事件（鼠标输入、按键等）时运行。 */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  ImGui_ImplSDL3_ProcessEvent(event); // 处理事件
  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS; /* 结束程序，向操作系统报告成功。 */
  }
  return SDL_APP_CONTINUE; /* 继续执行程序！ */
}

/* 此函数在每一帧运行一次，是程序的核心。 */
SDL_AppResult SDL_AppIterate(void *appstate) {

  /* 如果窗口已最小化，则等待。 */
  if (SDL_GetWindowFlags(窗口) & SDL_WINDOW_MINIMIZED) {
    SDL_Delay(10);
    return SDL_APP_CONTINUE;
  }

  // 开始Dear ImGui帧
  ImGui_ImplSDLRenderer3_NewFrame();
  ImGui_ImplSDL3_NewFrame();
  ImGui::NewFrame();
  /* 在这里绘制您的应用程序！ */

  ImGui::ShowDemoWindow(); // 显示Dear ImGui演示窗口

  /* 在这里绘制您的应用程序！ */
  ImGui::Render(); // 渲染 Dear ImGui 内容

  /* 清除窗口到绘制颜色。 */
  SDL_SetRenderDrawColor(渲染器, 0, 0, 0, 255);
  SDL_RenderClear(渲染器);

  // 渲染 Dear ImGui 内容
  ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), 渲染器);
  /* 将新清除的渲染放到屏幕上。 */
  SDL_RenderPresent(渲染器);

  return SDL_APP_CONTINUE; /* 继续执行程序！ */
}

/* 此函数在关闭时运行一次。 */
void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  /* SDL将为我们清理窗口/渲染器。 */

  ImGui_ImplSDLRenderer3_Shutdown();
  ImGui_ImplSDL3_Shutdown();
  ImGui::DestroyContext();

  SDL_DestroyRenderer(渲染器);
  SDL_DestroyWindow(窗口);
  SDL_Quit();
}
