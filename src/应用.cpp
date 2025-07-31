#include "应用.hpp"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_stdinc.h"
#include "icon/IconsFontAwesome6.h"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include "imsearch/imsearch.h"
#include <memory>
#include <string>
#include <vector>

std::unique_ptr<应用> 应用::_应用实例 = nullptr;
static const std::vector<std::string> 条目表{
    "选项1",
    "一二三",
    "选项a",
};
static std::string 选定字符 = 条目表.at(0);

void 应用::初始化() {
  if (!_应用实例) {
    _应用实例 = std::unique_ptr<应用>(new 应用());
  } else {
    SDL_Log("应用已经初始化");
  }
}
应用 &应用::获取实例() { return *_应用实例; }

void 应用::销毁实例() { _应用实例.reset(); }

bool 应用::是否已退出() { return _是否退出; }

应用::应用() {
  _帧延迟纳秒 = 1.0e9 / _目标帧率;

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
  float 字体大小 = 20.0f;
  ImFont *字体 = io.Fonts->AddFontFromFileTTF(
      "./assets/MapleMono-NF-CN-Regular.ttf", 字体大小);
  // 合并图标字体
  float 图标字体大小 = 字体大小 * 2.0f / 3.0f;
  // ImFontConfig 图标配置;
  // 图标配置.MergeMode = true;
  // 图标配置.PixelSnapH = true;
  // 图标配置.GlyphMinAdvanceX = 图标字体大小;
  ImFont *图标字体 =
      io.Fonts->AddFontFromFileTTF("./assets/fa-solid-900.ttf", 字体大小);
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
void 应用::更新() {

  auto 起始 = SDL_GetTicksNS();

  // 开始Dear ImGui帧
  ImGui_ImplSDLRenderer3_NewFrame();
  ImGui_ImplSDL3_NewFrame();
  ImGui::NewFrame();
  /* 绘制 */

  ImGui::Begin(ICON_FA_BLENDER "窗口");
  ImGui::Text(ICON_FA_PEN " 帧率: %f 每秒", 1.0 / _帧间隔时长);
  ImGui::End();

  ImSearch::ShowDemoWindow();
  ImGui::ShowDemoWindow();

  // 搜索示例
  if (ImGui::BeginCombo("##Extensions", 选定字符.c_str())) {
    if (ImSearch::BeginSearch()) {
      ImSearch::SearchBar("搜索栏");
      for (auto 条目项 : 条目表) {
        ImSearch::SearchableItem(条目项.c_str(), [&](const char *条目名) {
          const bool 是否选中 = 条目名 == 选定字符.c_str();
          if (ImGui::Selectable(条目名, 是否选中)) {
            选定字符 = 条目名;
          }
        });
      }
      ImSearch::EndSearch();
    }
    ImGui::EndCombo();
  }

  SDL_SetRenderDrawColor(_渲染器, 0, 0, 0, 255); // 设置渲染背景色
  SDL_RenderClear(_渲染器);                      // 清屏

  /* 绘制 */
  // 渲染呈现
  ImGui::Render();
  ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), _渲染器);
  SDL_RenderPresent(_渲染器); // 渲染

  // 帧率控制
  auto 结束 = SDL_GetTicksNS(); // 获取当前时间
  auto 耗时 = (结束 - 起始);    // 计算耗时
  if (耗时 < _帧延迟纳秒) {
    // 如果耗时小于帧延迟，则等待剩余时间
    SDL_DelayNS((_帧间隔时长 - 耗时) / 1000000); // ns纳秒
    _帧间隔时长 = _帧延迟纳秒 / 1.0e9;           // 将帧延迟转换为秒
  } else {
    // 如果耗时大于帧延迟，则调整帧间隔
    _帧间隔时长 = 耗时 / 1.0e9; // 将耗时转换为秒
  }
  // SDL_Log("帧率: %f 每秒", 1.0 / _帧间隔时长);
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