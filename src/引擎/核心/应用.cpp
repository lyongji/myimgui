#include "应用.hpp"
#include "SDL3/SDL_blendmode.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_video.h"
#include "icon/IconsFontAwesome6.h"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include "imsearch/imsearch.h"
#include "日志.hpp"
#include "时间.hpp"
#include "渲染器.hpp"
#include "窗口.hpp"
#include "资源管理器.hpp"
#include "颜色.hpp"
#include <limits.h>
#include <memory>

namespace 引擎::核心 {
std::unique_ptr<应用> 应用::_应用实例;
应用::应用() = default;
应用::~应用() { 关闭组件(); }
应用 &应用::获取实例() { return *_应用实例; }
void 应用::销毁实例() { _应用实例.reset(); }
bool 应用::初始化() {
  if (!_应用实例) {
    _应用实例 = std::unique_ptr<应用>(new 应用());
    return _应用实例->执行组件初始化();
  } else {
    记录警告("初始化上下文单例两次！");
  }
  记录信息("应用初始化成功。");
  return true;
}

bool 应用::执行组件初始化() {
  if (!_应用实例->初始化SDL())
    return false;
  if (!_应用实例->初始化ImGui())
    return false;
  if (!_应用实例->初始化时间())
    return false;
  if (!_应用实例->初始化资源管理器())
    return false;

  测试资源管理器();
  记录跟踪("组件初始化成功。");
  return true;
}

void 应用::关闭组件() {
  // 按照依赖逆序关闭组件
  _资源管理器->清空资源();
  _资源管理器.reset(); // 销毁资源管理器
  // 是你的SDL_AppQuit()函数
  ImGui_ImplSDLRenderer3_Shutdown();
  ImGui_ImplSDL3_Shutdown();
  ImSearch::DestroyContext();
  ImGui::DestroyContext();

  _时间.reset();   // 销毁时间
  _渲染器.reset(); // 销毁渲染器
  _窗口.reset();   // 销毁窗口
  SDL_Quit();
}

bool 应用::初始化SDL() {
  // 初始化SDL
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD | SDL_INIT_AUDIO)) {
    记录错误("初始化SDL失败: {}", SDL_GetError());
  }

  // 创建窗口和渲染器
  Uint32 窗口标志 =
      SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY;
  _窗口 = std::make_unique<窗口>("标题", 1080, 720, 窗口标志);
  if (_窗口 == nullptr) {
    记录错误("无法创建窗口! SDL错误: {}", SDL_GetError());
    return false;
  }
  _渲染器 = std::make_unique<渲染::渲染器>(*_窗口);
  if (_渲染器 == nullptr) {
    记录错误("无法创建渲染器! SDL错误: {}", SDL_GetError());
    return false;
  }

  // 配置渲染器
  _渲染器->设置混合模式(SDL_BLENDMODE_BLEND);
  _渲染器->设置Vsync(true);

  // SDL_SetRenderLogicalPresentation(_渲染器->获取渲染器(), 1080,
  //                                  720, // 固定逻辑分辨率
  //                                  SDL_LOGICAL_PRESENTATION_LETTERBOX);
  _窗口->居中();

  // DPI设置
  _显示比例 = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
  记录信息("显示比例: {}  ", _显示比例);

  _窗口->显示();
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
    io.ConfigWindowsMoveFromTitleBarOnly = true; // 只能通过标题栏移动窗口
    io.ConfigDockingWithShift = false;           // 不需要按住Shift才能停靠
    // 设置Dear ImGui样式
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsLight();
    // 根据DPI缩放样式
    if (_显示比例 != 1.0f) {
      ImGui::GetStyle().ScaleAllSizes(_显示比例); // 缩放所有样式元素
      ImGui::GetStyle().FontScaleDpi = _显示比例; // 缩放字体
    }
    _渲染器->设置缩放(
        ImGui::GetIO().DisplayFramebufferScale.x,
        ImGui::GetIO().DisplayFramebufferScale.y); // 设置渲染器缩放
    // 设置平台/渲染器后端
    ImGui_ImplSDL3_InitForSDLRenderer(_窗口->获取窗口(), _渲染器->获取渲染器());
    ImGui_ImplSDLRenderer3_Init(_渲染器->获取渲染器());

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
    _时间 = std::make_unique<时间>();
  } catch (const std::exception &e) {
    记录错误("初始化时间管理失败: {}", e.what());
    return false;
  }
  _时间->设置目标帧率(60); // 临时硬编码
  记录跟踪("时间管理初始化成功。");
  return true;
}

bool 应用::初始化资源管理器() {
  try {
    _资源管理器 =
        std::make_unique<引擎::资源::资源管理器>(_渲染器->获取渲染器());
  } catch (const std::exception &e) {
    记录错误("初始化资源管理器失败: {}", e.what());
    return false;
  }

  记录跟踪("资源管理器初始化成功。");
  return true;
}

void 应用::处理事件(SDL_Event &事件) {

  ImGui_ImplSDL3_ProcessEvent(&事件);
  if (事件.type == SDL_EVENT_QUIT)
    _是否退出 = true;
  if (事件.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED &&
      事件.window.windowID == SDL_GetWindowID(_窗口->获取窗口()))
    _是否退出 = true;
}
void 应用::更新逻辑(float 帧间隔时长) {
  // 更新游戏逻辑
  // TODO: 在这里添加游戏逻辑更新代码
}
void 应用::更新UI() {
  // 开始Dear ImGui帧
  ImGui_ImplSDLRenderer3_NewFrame();
  ImGui_ImplSDL3_NewFrame();
  ImGui::NewFrame();

  // 绘制UI
  绘制UI();

  // 渲染呈现
  ImGui::Render();
}
void 应用::绘制UI() {
  ImGui::Begin(ICON_FA_BLENDER "窗口" ICON_FA_PEN "应用程序");
  ImGui::Text(ICON_FA_PEN " 帧率: %d FPS 每秒 (%.6f ms)", _时间->获取目标帧率(),
              _时间->获取无缩放帧间时长());

  ImGui::End();

  ImSearch::ShowDemoWindow(); // 显示搜索示例窗口
  ImGui::ShowDemoWindow();    // 显示Dear ImGui示例窗口
}
void 应用::绘制画面() {

  _渲染器->清屏(变量::颜色::黝黑());
  // 渲染ImGui绘制数据
  ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(),
                                        _渲染器->获取渲染器());

  // 呈现渲染结果
  _渲染器->渲染呈现();
}

void 应用::运行() {
  _时间->更新();
  auto 帧间隔时长 = _时间->获取帧间时长();
  更新逻辑(帧间隔时长);
  更新UI();
  绘制画面();
}

bool 应用::是否已退出() { return _是否退出; }
void 应用::测试资源管理器() {
  测试纹理();
  测试音频();
  测试字体();
}

void 应用::测试纹理() {
  _资源管理器->获取纹理("D:/MyData/cpp/我的imgui/assets/Preview.png");
  _资源管理器->卸载纹理("D:/MyData/cpp/我的imgui/assets/Preview.png");
}
void 应用::测试音频() {
  _资源管理器->获取音效("D:/MyData/cpp/我的imgui/assets/button_click.wav");
  _资源管理器->卸载音效("D:/MyData/cpp/我的imgui/assets/button_click.wav");
}
void 应用::测试字体() {
  _资源管理器->获取字体(
      "D:/MyData/cpp/我的imgui/assets/MapleMono-NF-CN-Regular.ttf", 10);
  _资源管理器->卸载字体(
      "D:/MyData/cpp/我的imgui/assets/MapleMono-NF-CN-Regular.ttf", 10);
}

} // namespace 引擎::核心