#include "纹理管理器.hpp"
#include "SDL3/SDL_render.h"
#include "SDL3_image/SDL_image.h"
#include "日志.hpp"

namespace 引擎::资源 {

纹理管理器::纹理管理器(SDL_Renderer *渲染器) : _渲染器(渲染器) {
  if (!_渲染器) {
    throw std::runtime_error("纹理管理器初始化失败，渲染器为空");
  }
  记录跟踪("纹理管理器初始化成功");
}
SDL_Texture *纹理管理器::载入纹理(std::string_view 路径) {
  // 1. 检查路径有效性
  if (路径.empty()) {
    记录错误("纹理路径为空");
    return nullptr;
  }

  // 3. 检查是否已缓存
  auto it = _纹理池.find(std::string(路径));
  if (it != _纹理池.end()) {
    return it->second.get();
  }

  // 4. 加载纹理
  SDL_Texture *纹理 = IMG_LoadTexture(_渲染器, 路径.data());
  if (!纹理) {
    记录错误("加载纹理失败: {} (原因: {})", 路径, SDL_GetError());
    return nullptr;
  }

  // 5. 设置纹理参数
  if (!SDL_SetTextureScaleMode(纹理, SDL_SCALEMODE_NEAREST)) {
    记录警告("无法设置纹理缩放模式: {}", SDL_GetError());
  }

  // 6. 存入缓存
  _纹理池.emplace(路径, std::unique_ptr<SDL_Texture, SDLTexture删除器>(纹理));

  记录跟踪("成功加载并缓存纹理: {}", 路径);
  return 纹理;
}
SDL_Texture *纹理管理器::获取纹理(std::string_view 路径) {
  if (路径.empty()) {
    记录错误("纹理路径为空");
    return nullptr;
  }

  // 查找现有纹理
  auto it = _纹理池.find(std::string(路径));
  if (it != _纹理池.end()) {
    return it->second.get();
  }

  // 如果未找到，尝试加载它
  记录警告("纹理 '{}' 未找到缓存，尝试加载。", 路径);
  auto 纹理 = 载入纹理(路径);

  // 检查加载是否成功
  if (!纹理) {
    记录错误("无法加载纹理: {}", 路径);
    return nullptr;
  }

  return 纹理;
}

glm::vec2 纹理管理器::获取纹理尺寸(std::string_view 路径) {
  // 获取纹理
  SDL_Texture *纹理 = 获取纹理(路径);
  if (!纹理) {
    记录错误("无法获取纹理: {}", 路径);
    return glm::vec2(0);
  }

  // 2. 获取尺寸（正确用法）
  float 宽度 = 0, 高度 = 0;
  SDL_GetTextureSize(纹理, &宽度, &高度);

  return glm::vec2(宽度, 高度);
}
void 纹理管理器::卸载纹理(std::string_view 路径) {
  auto it = _纹理池.find(std::string(路径));
  if (it != _纹理池.end()) {
    记录调试("卸载纹理: {}", 路径);
    _纹理池.erase(it); // unique_ptr 通过自定义删除器处理删除
  } else {
    记录警告("尝试卸载不存在的纹理: {}", 路径);
  }
}
void 纹理管理器::清空纹理池() {
  if (!_纹理池.empty()) {
    记录调试("正在清除所有 {} 个缓存的纹理。", _纹理池.size());
    _纹理池.clear(); // unique_ptr 处理所有元素的删除
  }
}
} // namespace 引擎::资源