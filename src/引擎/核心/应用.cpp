#include "应用.hpp"
#include "SDL3/SDL_stdinc.h"
#include "icon/IconsFontAwesome6.h"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include "imsearch/imsearch.h"
#include "日志.hpp"
#include <memory>

std::unique_ptr<应用> 应用::_应用实例 = nullptr;

void 应用::初始化() {
  if (!_应用实例) {
    _应用实例 = std::unique_ptr<应用>(new 应用());
    记录信息("应用初始化成功。");
  } else {
    记录信息("应用已初始化过。");
  }
}
应用 &应用::获取实例() { return *_应用实例; }

void 应用::销毁实例() { _应用实例.reset(); }

bool 应用::是否已退出() { return _是否退出; }

应用::应用() {

  // 初始化SDL
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD | SDL_INIT_AUDIO);

  float dpi系数 = SDL_GetDisplayContentScale(1);
  // 创建窗口和渲染器
  Uint32 窗口标志 =
      SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY;
  _窗口 = SDL_CreateWindow("标题", 1920, 1080, 窗口标志);
  _渲染器 = SDL_CreateRenderer(_窗口, nullptr);

  SDL_SetRenderVSync(_渲染器, SDL_RENDERER_VSYNC_ADAPTIVE); // 设置垂直同步
  SDL_SetWindowPosition(_窗口, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
  SDL_ShowWindow(_窗口);
  // ================= 添加DPI支持 =================
  // 1. 获取初始显示比例
  _显示比例 = SDL_GetWindowDisplayScale(_窗口);

  // 设置Dear ImGui 环境
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImSearch::CreateContext(); // 初始化搜索
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // 启用键盘控制
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // 启用游戏手柄控制
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // 启用停靠

  // 设置Dear ImGui样式
  ImGui::StyleColorsDark();
  // ImGui::StyleColorsLight();
  // 根据DPI缩放样式
  ImGui::GetStyle().ScaleAllSizes(_显示比例); // 缩放所有样式元素
  ImGui::GetStyle().FontScaleDpi = _显示比例; // 缩放字体

  // 设置平台/渲染器后端
  ImGui_ImplSDL3_InitForSDLRenderer(_窗口, _渲染器);
  ImGui_ImplSDLRenderer3_Init(_渲染器);

  // 设置字体
  static constexpr int 字体大小 = 20;
  // 1. 加载主字体（排除图标范围）
  static const ImWchar 排除范围[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
  ImFontConfig 主字体配置;
  主字体配置.GlyphExcludeRanges = 排除范围; // 排除图标范围

  ImFont *字体 = io.Fonts->AddFontFromFileTTF(
      "./assets/MapleMono-NF-CN-Regular.ttf", 字体大小, &主字体配置);
  // 合并图标字体
  // 合并图标字体时需指定范围
  static const ImWchar 图标范围[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
  float 图标字体大小 = 字体大小 * 2.0f / 3.0f;
  ImFontConfig 图标配置;
  图标配置.MergeMode = true;
  图标配置.GlyphOffset.y = 1.0f;
  图标配置.GlyphMinAdvanceX = 图标字体大小; // 使图标水平对齐
  图标配置.GlyphRanges = 图标范围;          // 指定图标范围
  ImFont *图标字体 = io.Fonts->AddFontFromFileTTF("./assets/fa-solid-900.ttf",
                                                  字体大小, &图标配置);
  IM_ASSERT(字体 != nullptr);
}
void 应用::处理事件(SDL_Event &事件) {

  ImGui_ImplSDL3_ProcessEvent(&事件);
  if (事件.type == SDL_EVENT_QUIT)
    _是否退出 = true;
  if (事件.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED &&
      事件.window.windowID == SDL_GetWindowID(_窗口))
    _是否退出 = true;
}
void 应用::更新迭代(float 帧间隔时长) {

  // 开始Dear ImGui帧
  ImGui_ImplSDLRenderer3_NewFrame();
  ImGui_ImplSDL3_NewFrame();
  ImGui::NewFrame();
  /* 绘制 */

  ImGui::Begin(ICON_FA_BLENDER "窗口" ICON_FA_PEN "应用程序");
  ImGui::Text(ICON_FA_PEN " 帧率: %f 每秒", 1.0 / 帧间隔时长);
  ImGui::End();

  ImSearch::ShowDemoWindow(); // 显示搜索示例窗口
  ImGui::ShowDemoWindow();    // 显示Dear ImGui示例窗口

  SDL_SetRenderDrawColor(_渲染器, 0, 0, 0, 255); // 设置渲染背景色
  SDL_RenderClear(_渲染器);                      // 清屏

  // 渲染呈现
  ImGui::Render();
}

void 应用::绘制画面() {
  ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), _渲染器);
  SDL_RenderPresent(_渲染器); // 渲染
}

void 应用::运行() {

  更新迭代(0);
  绘制画面();
}

应用::~应用() {
  // 清理
  // 是你的SDL_AppQuit()函数
  ImGui_ImplSDLRenderer3_Shutdown();
  ImGui_ImplSDL3_Shutdown();
  ImSearch::DestroyContext();
  ImGui::DestroyContext();

  SDL_DestroyRenderer(_渲染器);
  SDL_DestroyWindow(_窗口);

  SDL_Quit();
}