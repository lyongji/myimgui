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
  // 检查是否已加载
  auto it = _纹理池.find(std::string(路径)); // 查找路径
  if (it != _纹理池.end()) {
    return it->second.get();
  }
  // 如果没加载则尝试加载纹理
  SDL_Texture *原始纹理 = IMG_LoadTexture(_渲染器, 路径.data()); // 载入纹理
  // 载入纹理时，设置纹理缩放模式为最邻近插值(必不可少，否则TileLayer渲染中会出现边缘空隙/模糊)
  if (!SDL_SetTextureScaleMode(原始纹理, SDL_SCALEMODE_NEAREST)) {
    记录警告("无法设置纹理缩放模式为最邻近插值");
  }

  if (!原始纹理) {
    记录错误("加载纹理失败: '{}': {}", 路径, SDL_GetError());
    return nullptr;
  }

  // 使用带有自定义删除器的 unique_ptr 存储加载的纹理
  _纹理池.emplace(路径,
                  std::unique_ptr<SDL_Texture, SDLTexture删除器>(原始纹理));
  记录调试("成功加载并缓存纹理: {}", 路径);

  return 原始纹理;
}
SDL_Texture *纹理管理器::获取纹理(std::string_view 路径) {
  // 查找现有纹理
  auto it = _纹理池.find(std::string(路径));
  if (it != _纹理池.end()) {
    return it->second.get();
  }

  // 如果未找到，尝试加载它
  记录警告("纹理 '{}' 未找到缓存，尝试加载。", 路径);
  return 载入纹理(路径);
}
glm::vec2 纹理管理器::获取纹理尺寸(std::string_view 路径) {
  // 获取纹理
  SDL_Texture *纹理 = 获取纹理(路径);
  if (!纹理) {
    记录错误("无法获取纹理: {}", 路径);
    return glm::vec2(0);
  }

  // 获取纹理尺寸
  glm::vec2 尺寸;
  if (!SDL_GetTextureSize(纹理, &尺寸.x, &尺寸.y)) {
    记录错误("无法查询纹理尺寸: {}", 路径);
    return glm::vec2(0);
  }
  return 尺寸;
}
void 纹理管理器::卸载纹理(std::string_view 路径) {
  auto it = _纹理池.find(std::string(路径));
  if (it != _纹理池.end()) {
    spdlog::debug("卸载纹理: {}", 路径);
    _纹理池.erase(it); // unique_ptr 通过自定义删除器处理删除
  } else {
    spdlog::warn("尝试卸载不存在的纹理: {}", 路径);
  }
}
void 纹理管理器::清空纹理池() {
  if (!_纹理池.empty()) {
    spdlog::debug("正在清除所有 {} 个缓存的纹理。", _纹理池.size());
    _纹理池.clear(); // unique_ptr 处理所有元素的删除
  }
}
} // namespace 引擎::资源