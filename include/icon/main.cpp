// Dear ImGui: standalone example application for SDL3 + SDL_Renderer
// (SDL is a cross-platform general purpose library for handling windows,
// inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)

// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/
// folder).
// - Introduction, links and more at the top of imgui.cpp

// Important to understand: SDL_Renderer is an _optional_ component of SDL3.
// For a multi-platform app consider using e.g. SDL+DirectX on Windows and
// SDL+OpenGL on Linux/OSX.

#include "IconsFontAwesome6.h"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include <SDL3/SDL.h>
#include <stdio.h>

#ifdef __EMSCRIPTEN__
#include "../libs/emscripten/emscripten_mainloop_stub.h"
#endif

// 主代码
int main(int, char **) {
  // 设置SDL
  // [如果使用SDL_MAIN_USE_CALLBACKS：下面的所有代码直到主循环开始
  // 可能是你的SDL_AppInit()函数]
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
    printf("错误: SDL_Init(): %s\n", SDL_GetError());
    return -1;
  }

  // 创建带有SDL_Renderer图形上下文的窗口
  Uint32 window_flags =
      SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY;
  SDL_Window *window = SDL_CreateWindow("Dear ImGui SDL3+SDL_Renderer 示例",
                                        1280, 720, window_flags);

  printf("%f\n", SDL_GetDisplayContentScale(1));

  if (window == nullptr) {
    printf("错误: SDL_CreateWindow(): %s\n", SDL_GetError());
    return -1;
  }
  SDL_Renderer *renderer = SDL_CreateRenderer(window, nullptr);
  SDL_SetRenderVSync(renderer, 1);
  if (renderer == nullptr) {
    SDL_Log("错误: SDL_CreateRenderer(): %s\n", SDL_GetError());
    return -1;
  }
  SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
  SDL_ShowWindow(window);

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
  ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
  ImGui_ImplSDLRenderer3_Init(renderer);

  // 加载字体
  // - 如果没有加载字体，dear imgui将使用默认字体。你也可以
  // 加载多个字体并使用ImGui::PushFont()/PopFont()来选择
  // 它们。
  // - AddFontFromFileTTF()将返回ImFont*，因此如果你需要
  // 在多个字体中选择字体，可以存储它。
  // - 如果文件无法加载，该函数将返回一个空指针。请在
  // 你的应用程序中处理这些错误（例如使用断言，或显示
  // 错误并退出）。
  // -
  // 字体将在调用ImFontAtlas::Build()/GetTexDataAsXXXX()时以给定大小（带过采样）光栅化
  // 并存储到纹理中，ImGui_ImplXXXX_NewFrame以下将调用。
  // - 在你的imconfig文件中使用'#define IMGUI_ENABLE_FREETYPE'以使用Freetype
  // 进行更高品质的字体渲染。
  // - 阅读'docs/FONTS.md'以获取更多指令和详细信息。
  // - 请记住，在C/C++中，如果你想包含一个反斜杠\在字符串
  // 字面量中，你需要写一个双反斜杠\\ !
  // -
  // 我们的Emscripten构建过程允许从"fonts/"文件夹嵌入字体以在运行时访问。查看Makefile.emscripten了解详情。
  // io.Fonts->AddFontDefault();
  // io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
  // io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
  // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
  // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);

  float 基本字体大小 = 18.0f;

  ImFont *font =
      io.Fonts->AddFontFromFileTTF("./assets/MapleMono-NF-CN-Regular.ttf");

  io.Fonts->AddFontFromFileTTF("./assets/fa-solid-900.ttf");
  IM_ASSERT(font != nullptr);

  // 我们的状态

  bool show_demo_window = true;
  bool show_another_window = false;
  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  // 主循环
  bool done = false;
#ifdef __EMSCRIPTEN__
  // 对于Emscripten构建，我们禁用了文件系统访问，因此不要尝试
  // 对imgui.ini文件进行fopen()。你可以手动调用
  // LoadIniSettingsFromMemory()从自己的存储中加载设置。
  io.IniFilename = nullptr;
  EMSCRIPTEN_MAINLOOP_BEGIN
#else
  while (!done)
#endif
  {
    // 轮询和处理事件（输入，窗口大小调整等）
    // 你可以读取io.WantCaptureMouse，io.WantCaptureKeyboard标志来
    // 告诉dear imgui是否要使用你的输入。
    // -
    // 当io.WantCaptureMouse为true时，不要将鼠标输入数据分派到你的主应用程序，或清除/覆盖你的鼠标数据副本。
    // -
    // 当io.WantCaptureKeyboard为true时，不要将键盘输入数据分派到你的主应用程序，或清除/覆盖你的键盘数据副本。
    // 通常，你可以始终将所有输入传递给dear
    // imgui，并根据这两个标志从你的应用程序中隐藏它们。 [如果使用
    // SDL_MAIN_USE_CALLBACKS:
    // 从你的SDL_AppEvent()函数中调用ImGui_ImplSDL3_ProcessEvent()]
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      ImGui_ImplSDL3_ProcessEvent(&event);
      if (event.type == SDL_EVENT_QUIT)
        done = true;
      if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED &&
          event.window.windowID == SDL_GetWindowID(window))
        done = true;
    }

    // [如果使用SDL_MAIN_USE_CALLBACKS：以下代码很可能
    // 是你的SDL_AppIterate()函数]
    if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED) {
      SDL_Delay(10);
      continue;
    }

    // 开始Dear ImGui帧
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    // 1. 显示大演示窗口（大多数示例代码在
    // ImGui::ShowDemoWindow()中！你可以浏览其代码以了解更多关于Dear
    // ImGui的信息！）
    if (show_demo_window)
      ImGui::ShowDemoWindow(&show_demo_window);

    // 2. 显示一个我们自行创建的简单窗口。我们使用Begin/End对
    // 创建一个命名窗口。
    {
      static float f = 0.0f;
      static int counter = 0;

      ImGui::Begin(ICON_FA_ARROW_TREND_DOWN
                   "你好，世界！"); // 创建一个名为"你好，世界！"的窗口
                                    // 并在其中追加内容。

      ImGui::Text(ICON_FA_GEAR
                  "这是一些有用的文本。"); // 显示一些文本（你也可以
                                           // 使用格式化字符串）
      ImGui::Checkbox(
          "演示窗口",
          &show_demo_window); // 编辑布尔变量存储我们的窗口打开/关闭状态
      ImGui::Checkbox("另一个窗口", &show_another_window);

      ImGui::SliderFloat("浮点数", &f, 0.0f,
                         1.0f); // 使用滑动条编辑1个浮点数，范围从0.0f到1.0f
      ImGui::ColorEdit3("清除颜色",
                        (float *)&clear_color); // 编辑3个浮点数表示颜色

      if (ImGui::Button("按钮")) // 按钮在点击时返回true（大多数
                                 // 小部件在编辑/激活时返回true）
        counter++;
      ImGui::SameLine();
      ImGui::Text("计数器 = %d", counter);

      ImGui::Text("应用程序平均每帧%.3f毫秒（%.1f FPS）",
                  1000.0f / io.Framerate, io.Framerate);
      ImGui::End();
    }

    // 3. 显示另一个简单窗口。
    if (show_another_window) {
      ImGui::Begin(
          "另一个窗口",
          &show_another_window); // 传递指向我们的布尔变量的指针（窗口将有一个关闭按钮，
                                 // 点击时将清除布尔变量）
      ImGui::Text("来自另一个窗口的问候！");
      if (ImGui::Button("关闭我"))
        show_another_window = false;
      ImGui::End();
    }

    // 渲染
    ImGui::Render();
    // SDL_RenderSetScale(renderer, io.DisplayFramebufferScale.x,
    // io.DisplayFramebufferScale.y);
    SDL_SetRenderDrawColorFloat(renderer, clear_color.x, clear_color.y,
                                clear_color.z, clear_color.w);
    SDL_RenderClear(renderer);
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
    SDL_RenderPresent(renderer);
  }
#ifdef __EMSCRIPTEN__
  EMSCRIPTEN_MAINLOOP_END;
#endif

  // 清理
  // [如果使用SDL_MAIN_USE_CALLBACKS：以下代码很可能
  // 是你的SDL_AppQuit()函数]
  ImGui_ImplSDLRenderer3_Shutdown();
  ImGui_ImplSDL3_Shutdown();
  ImGui::DestroyContext();

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
