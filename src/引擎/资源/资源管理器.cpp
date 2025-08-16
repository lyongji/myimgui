#include "资源管理器.hpp"
#include "字体管理器.hpp"
#include "日志.hpp"
#include "纹理管理器.hpp"
#include "音频管理器.hpp"
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <glm/glm.hpp>
#include <string_view>

namespace 引擎::资源 {

资源管理器::~资源管理器() = default;

资源管理器::资源管理器(SDL_Renderer *渲染器) {
  // --- 初始化各个子系统 --- (如果出现错误会抛出异常，由上层捕获)
  _纹理管理器 = std::make_unique<纹理管理器>(渲染器);
  _音频管理器 = std::make_unique<音频管理器>();
  _字体管理器 = std::make_unique<字体管理器>();

  记录跟踪("资源管理器 构造成功。");
  // RAII:
  // 构造成功即代表资源管理器可以正常工作，无需再初始化，无需检查指针是否为空
}

void 资源管理器::清空资源() {
  _字体管理器->清空字体池();
  // _音频管理器->清空音频池();
  _纹理管理器->清空纹理池();
  记录跟踪("资源管理器 中的资源通过 clear() 清空。");
}

// --- 纹理接口实现 ---
SDL_Texture *资源管理器::载入纹理(std::string_view 文件路径) {
  // 构造函数已经确保了 _纹理管理器
  // 不为空，因此不需要再进行if检查，以免性能浪费
  return _纹理管理器->载入纹理(文件路径);
}

SDL_Texture *资源管理器::获取纹理(std::string_view 文件路径) {
  return _纹理管理器->获取纹理(文件路径);
}

glm::vec2 资源管理器::获取纹理尺寸(std::string_view 文件路径) {
  return _纹理管理器->获取纹理尺寸(文件路径);
}

void 资源管理器::卸载纹理(std::string_view 文件路径) {
  _纹理管理器->卸载纹理(文件路径);
}

void 资源管理器::清空纹理池() { _纹理管理器->清空纹理池(); }

// --- 音频接口实现 ---
Mix_Chunk *资源管理器::载入音效(std::string_view 文件路径) {
  return _音频管理器->载入音效(文件路径);
}

Mix_Chunk *资源管理器::获取音效(std::string_view 文件路径) {
  return _音频管理器->获取音效(文件路径);
}

void 资源管理器::卸载音效(std::string_view 文件路径) {
  _音频管理器->卸载音效(文件路径);
}

void 资源管理器::清空音效池() { _音频管理器->清空音效池(); }

Mix_Music *资源管理器::载入音乐(std::string_view 文件路径) {
  return _音频管理器->载入音乐(文件路径);
}

Mix_Music *资源管理器::获取音乐(std::string_view 文件路径) {
  return _音频管理器->获取音乐(文件路径);
}

void 资源管理器::卸载音乐(std::string_view 文件路径) {
  _音频管理器->卸载音乐(文件路径);
}

void 资源管理器::清空音乐池() { _音频管理器->清空音乐池(); }

// --- 字体接口实现 ---
TTF_Font *资源管理器::载入字体(std::string_view 文件路径, int 字号大小) {
  return _字体管理器->载入字体(文件路径, 字号大小);
}

TTF_Font *资源管理器::获取字体(std::string_view 文件路径, int 字号大小) {
  return _字体管理器->获取字体(文件路径, 字号大小);
}

void 资源管理器::卸载字体(std::string_view 文件路径, int 字号大小) {
  _字体管理器->卸载字体(文件路径, 字号大小);
}

void 资源管理器::清空字体池() { _字体管理器->清空字体池(); }

} // namespace 引擎::资源