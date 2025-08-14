#pragma once
#include "Path.hpp"
#include <memory>        // 用于 std::unique_ptr
#include <stdexcept>     // 用于 std::runtime_error
#include <string>        // 用于 std::string
#include <string_view>   // 用于 std::string_view 路径
#include <unordered_map> // 用于 std::unordered_map

#include <SDL3_mixer/SDL_mixer.h> // SDL_mixer 主头文件

namespace 引擎::资源 {

/**
 * @brief 管理 SDL_mixer 音效 (Mix_Chunk) 和音乐 (Mix_Music)。
 *
 * 提供音频资源的加载和缓存功能。构造失败时会抛出异常。
 * 仅供 资源管理器 内部使用。
 */
class 音频管理器 final {
  friend class 资源管理器;

private:
  // Mix_Chunk 的自定义删除器
  struct SDLMixChunk删除器 {
    void operator()(Mix_Chunk *音效) const {
      if (音效) {
        Mix_FreeChunk(音效);
      }
    }
  };

  // Mix_Music 的自定义删除器
  struct SDLMixMusic删除器 {
    void operator()(Mix_Music *音乐) const {
      if (音乐) {
        Mix_FreeMusic(音乐);
      }
    }
  };

  // 音效存储 (文件路径 -> Mix_Chunk)
  std::unordered_map<std::string, std::unique_ptr<Mix_Chunk, SDLMixChunk删除器>>
      _音效池;
  // 音乐存储 (文件路径 -> Mix_Music)
  std::unordered_map<std::string, std::unique_ptr<Mix_Music, SDLMixMusic删除器>>
      _音乐池;

public:
  /**
   * @brief 构造函数。初始化 SDL_mixer 并打开音频设备。
   * @throws std::runtime_error 如果 SDL_mixer 初始化或打开音频设备失败。
   */
  音频管理器();

  ~音频管理器(); //  需要手动添加析构函数，清理资源并关闭 SDL_mixer。

  // 当前设计中，我们只需要一个音频管理器，所有权不变，所以不需要拷贝、移动相关构造及赋值运算符
  音频管理器(const 音频管理器 &) = delete;
  音频管理器 &operator=(const 音频管理器 &) = delete;
  音频管理器(音频管理器 &&) = delete;
  音频管理器 &operator=(音频管理器 &&) = delete;

private: // 仅供 资源管理器 访问的方法
  Mix_Chunk *载入音效(std::string_view 路径);
  Mix_Chunk *
  获取音效(std::string_view 路径); // 尝试获取已加载音效的指针，未加载则尝试加载
  void 卸载音效(std::string_view 路径); // 卸载指定的音效资源
  void 清空音效池();                    // 清空所有音效资源

  Mix_Music *载入音乐(std::string_view 路径); // 从文件路径加载音乐
  Mix_Music *
  获取音乐(std::string_view 路径); // 尝试获取已加载音乐的指针，未加载则尝试加载
  void 卸载音乐(std::string_view 路径); // 卸载指定的音乐资源
  void 清空音乐池();                    // 清空所有音乐资源

  void 清空音频池(); // 清空所有音频资源
};

} // namespace 引擎::资源