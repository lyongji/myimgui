#include "日志.hpp"
#include "SDL3/SDL.h"
#include "spdlog/sinks/android_sink.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/null_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <print>

日志管理器 日志管理器::管理器实例;

日志管理器 &日志管理器::获取实例() { return 管理器实例; }

日志管理器::日志管理器() {
#ifdef SDL_PLATFORM_ANDROID
  std::string tag = "spdlog-android";
  _控制台日志 = spdlog::android_logger_mt("android", tag);

  _文件日志 = spdlog::null_logger_mt("文件日志");
#else
  _控制台日志 = spdlog::stdout_color_mt("控制台日志");

  try {
    _文件日志 = spdlog::basic_logger_mt("文件日志", "logs/log.txt");
  } catch (const spdlog::spdlog_ex &ex) {
    std::println("日志初始化失败: {}", ex.what());
  }
#endif
}