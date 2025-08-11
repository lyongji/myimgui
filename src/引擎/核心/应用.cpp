#include "应用.hpp"
#include "SDL3/SDL_oldnames.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_video.h"
#include "icon/IconsFontAwesome6.h"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include "imsearch/imsearch.h"
#include "日志.hpp"
#include <memory>

namespace 引擎::核心 {
std::unique_ptr<应用> 应用::_应用实例 = nullptr;

bool 应用::初始化() {
  记录跟踪("开始应用初始化。");
  if (!_应用实例) {
    try {
      _应用实例 = std::unique_ptr<应用>(new 应用());
      记录跟踪("应用实例创建成功。");
      if (!_应用实例->执行组件初始化())
        return false;
    } catch (const std::exception &e) {
      记录错误("创建应用实例失败: {}", e.what());
      return false;
    }
    记录信息("应用初始化成功。");
    return true;
  } else {
    记录信息("应用已初始化过。");
    return false;
  }
}

bool 应用::执行组件初始化() {
  if (!_应用实例->初始化ImGui())
    return false;

  if (!_应用实例->初始化时间())
    return false;

  记录跟踪("组件初始化成功。");
  return true;
}
bool 应用::初始化ImGui() {
  try {
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
    ImGui_ImplSDL3_InitForSDLRenderer(_窗口->获取窗口(), _渲染器);
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
  } catch (const std::exception &e) {
    记录错误(" 初始化ImGui失败: {}", e.what());
    return false;
  }
  return true;
}
bool 应用::初始化时间() {
  try {
    _时间 = std::unique_ptr<时间>(new 时间());
  } catch (const std::exception &e) {
    记录错误("初始化时间管理失败: {}", e.what());
    return false;
  }
  _时间->设置目标帧率(144); // 临时硬编码
  记录跟踪("时间管理初始化成功。");
  return true;
}
应用 &应用::获取实例() { return *_应用实例; }

void 应用::销毁实例() { _应用实例.reset(); }

bool 应用::是否已退出() { return _是否退出; }

应用::应用() {
  // 初始化SDL
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD | SDL_INIT_AUDIO)) {
    记录错误("初始化SDL失败: {}", SDL_GetError());
  }

  // 创建窗口和渲染器
  Uint32 窗口标志 = SDL_WINDOW_RESIZABLE |
                    SDL_WINDOW_HIDDEN; //| SDL_WINDOW_HIGH_PIXEL_DENSITY;
  _窗口 = std::make_unique<窗口>("标题", 1080, 720, 窗口标志);
  if (_窗口 == nullptr) {
    记录错误("无法创建窗口! SDL错误: {}", SDL_GetError());
  }
  _渲染器 = SDL_CreateRenderer(_窗口->获取窗口(), nullptr);
  if (_渲染器 == nullptr) {
    spdlog::error("无法创建渲染器! SDL错误: {}", SDL_GetError());
  }
  // 设置渲染器支持透明色
  SDL_SetRenderDrawBlendMode(_渲染器, SDL_BLENDMODE_BLEND);

  // 设置 VSync (注意: VSync
  // 开启时，驱动程序会尝试将帧率限制到显示器刷新率，有可能会覆盖我们手动设置的
  // 目标帧率)
  auto vsync_mode = SDL_RENDERER_VSYNC_ADAPTIVE | SDL_RENDERER_VSYNC_DISABLED;
  SDL_SetRenderVSync(_渲染器, vsync_mode); // 设置垂直同步
  // 记录跟踪(("VSync 设置为: {}", vsync_mode ? "Enabled" : "Disabled");

  SDL_SetWindowPosition(_窗口->获取窗口(), SDL_WINDOWPOS_CENTERED,
                        SDL_WINDOWPOS_CENTERED);

  // ================= 添加DPI支持 =================
  // 1. 获取主显示器的DPI系数
  auto i = SDL_GetPrimaryDisplay();
  float dpi系数 = SDL_GetDisplayContentScale(i);
  // _显示比例 = dpi系数;
  // 2. 获取显示比例
  _显示比例 = SDL_GetWindowDisplayScale(_窗口->获取窗口());
  记录信息("显示比例: {}", _显示比例);
  SDL_ShowWindow(_窗口->获取窗口());
}
void 应用::处理事件(SDL_Event &事件) {

  ImGui_ImplSDL3_ProcessEvent(&事件);
  if (事件.type == SDL_EVENT_QUIT)
    _是否退出 = true;
  if (事件.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED &&
      事件.window.windowID == SDL_GetWindowID(_窗口->获取窗口()))
    _是否退出 = true;
}
void 应用::更新迭代(float 帧间隔时长) {

  // 开始Dear ImGui帧
  ImGui_ImplSDLRenderer3_NewFrame();
  ImGui_ImplSDL3_NewFrame();
  ImGui::NewFrame();
  /* 绘制 */

  ImGui::Begin(ICON_FA_BLENDER "窗口" ICON_FA_PEN "应用程序");
  ImGui::Text(ICON_FA_PEN " 帧率: %d FPS 每秒 (%.6f ms)", _时间->获取目标帧率(),
              _时间->获取无缩放帧间时长());

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
  _时间->更新();
  auto 帧间隔时长 = _时间->获取帧间时长();
  更新迭代(帧间隔时长);
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
  _窗口.reset(); // 销毁窗口

  SDL_Quit();
}
} // namespace 引擎::核心