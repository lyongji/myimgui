#include "音频管理器.hpp"
#include "SDL3_mixer/SDL_mixer.h"
#include "日志.hpp"

namespace 引擎::资源 {
音频管理器::音频管理器() {
  // 使用所需的格式初始化SDL_mixer（推荐OGG、MP3）
  MIX_InitFlags 标志 = MIX_INIT_OGG | MIX_INIT_MP3;
  if ((Mix_Init(标志) & 标志) != 标志) {
    throw std::runtime_error("音频管理器 错误: Mix_Init 失败: " +
                             std::string(SDL_GetError()));
  }

  // SDL3打开音频设备的方法。默认值：44100
  // Hz，默认格式，2声道（立体声），2048采样块大小
  if (!Mix_OpenAudio(0, nullptr)) {
    Mix_Quit(); // 如果OpenAudio失败，先清理Mix_Init，再抛出异常
    throw std::runtime_error("音频管理器 错误: Mix_OpenAudio 失败: " +
                             std::string(SDL_GetError()));
  }
  记录跟踪("音频管理器 构造成功。");
}

音频管理器::~音频管理器() {
  // 立即停止所有音频播放
  Mix_HaltChannel(-1); // 停止所有音效
  Mix_HaltMusic();     // 停止音乐

  // 清理资源映射（unique_ptrs会调用删除器）
  清空音效池();
  清空音乐池();

  // 关闭音频设备
  Mix_CloseAudio();

  // 退出SDL_mixer子系统
  Mix_Quit();
  记录跟踪("音频管理器 析构成功。");
}

// --- 音效管理 ---
Mix_Chunk *音频管理器::载入音效(std::string_view 路径) {
  // 首先检查缓存
  auto it = _音效池.find(std::string(路径));
  if (it != _音效池.end()) {
    return it->second.get();
  }

  // 加载音效块
  记录调试("加载音效: {}", 路径);
  Mix_Chunk *原始音效 = Mix_LoadWAV(路径.data());
  if (!原始音效) {
    记录错误("加载音效失败: '{}': {}", 路径, SDL_GetError());
    return nullptr;
  }

  // 使用unique_ptr存储在缓存中
  _音效池.emplace(路径,
                  std::unique_ptr<Mix_Chunk, SDLMixChunk删除器>(原始音效));
  记录调试("成功加载并缓存音效: {}", 路径);
  return 原始音效;
}

Mix_Chunk *音频管理器::获取音效(std::string_view 路径) {
  auto it = _音效池.find(std::string(路径));
  if (it != _音效池.end()) {
    return it->second.get();
  }
  记录警告("音效 '{}' 未找到缓存，尝试加载。", 路径);
  return 载入音效(路径);
}

void 音频管理器::卸载音效(std::string_view 路径) {
  auto it = _音效池.find(std::string(路径));
  if (it != _音效池.end()) {
    记录调试("卸载音效: {}", 路径);
    _音效池.erase(it); // unique_ptr处理Mix_FreeChunk
  } else {
    记录警告("尝试卸载不存在的音效: {}", 路径);
  }
}

void 音频管理器::清空音效池() {
  if (!_音效池.empty()) {
    记录调试("正在清除所有 {} 个缓存的音效。", _音效池.size());
    _音效池.clear(); // unique_ptr处理删除
  }
}

// --- 音乐管理 ---
Mix_Music *音频管理器::载入音乐(std::string_view 路径) {
  // 首先检查缓存
  auto it = _音乐池.find(std::string(路径));
  if (it != _音乐池.end()) {
    return it->second.get();
  }

  // 加载音乐
  记录调试("加载音乐: {}", 路径);
  Mix_Music *原始音乐 = Mix_LoadMUS(路径.data());
  if (!原始音乐) {
    记录错误("加载音乐失败: '{}': {}", 路径, SDL_GetError());
    return nullptr;
  }

  // 使用unique_ptr存储在缓存中
  _音乐池.emplace(路径,
                  std::unique_ptr<Mix_Music, SDLMixMusic删除器>(原始音乐));
  记录调试("成功加载并缓存音乐: {}", 路径);
  return 原始音乐;
}

Mix_Music *音频管理器::获取音乐(std::string_view 路径) {
  auto it = _音乐池.find(std::string(路径));
  if (it != _音乐池.end()) {
    return it->second.get();
  }
  记录警告("音乐 '{}' 未找到缓存，尝试加载。", 路径);
  return 载入音乐(路径);
}

void 音频管理器::卸载音乐(std::string_view 路径) {
  auto it = _音乐池.find(std::string(路径));
  if (it != _音乐池.end()) {
    记录调试("卸载音乐: {}", 路径);
    _音乐池.erase(it); // unique_ptr处理Mix_FreeMusic
  } else {
    记录警告("尝试卸载不存在的音乐: {}", 路径);
  }
}

void 音频管理器::清空音乐池() {
  if (!_音乐池.empty()) {
    记录调试("正在清除所有 {} 个缓存的音乐曲目。", _音乐池.size());
    _音乐池.clear(); // unique_ptr处理删除
  }
}

void 音频管理器::清空音频池() {
  清空音效池();
  清空音乐池();
}

} // namespace 引擎::资源