#include "Path.hpp"
#include "SDL3/SDL.h"
#include "glm/ext/vector_float2.hpp"
#include "日志.hpp"

namespace 引擎::资源 {
/**
 * @brief 管理 SDL_Texture 资源的加载、存储和检索。
 *
 * 在构造时初始化。使用文件路径作为键，确保纹理只加载一次并正确释放。
 * 依赖于一个有效的 SDL_Renderer，构造失败会抛出异常。
 */
class 纹理管理器 final {
  friend class 资源管理器;

private:
  // SDL_Texture 的删除器函数对象，用于智能指针管理
  struct SDLTexture删除器 {
    void operator()(SDL_Texture *纹理) const {
      if (纹理) {
        SDL_DestroyTexture(纹理);
      }
    }
  };
  // 存储文件路径和指向管理纹理的 unique_ptr
  // 的映射。(容器的键不可使用std::string_view)
  std::unordered_map<Path, std::unique_ptr<SDL_Texture, SDLTexture删除器>>
      _纹理池;

  SDL_Renderer *_渲染器; // 指向主渲染器

public:
  explicit 纹理管理器(SDL_Renderer *渲染器);

  纹理管理器(纹理管理器 &&) = delete;
  纹理管理器(const 纹理管理器 &) = delete;
  纹理管理器 &operator=(纹理管理器 &&) = delete;
  纹理管理器 &operator=(const 纹理管理器 &) = delete;

private:
  // 提供访问方法
  SDL_Texture *载入纹理(std::string_view);
  SDL_Texture *获取纹理(std::string_view);
  glm::vec2 获取纹理尺寸(std::string_view);
  void 卸载纹理(std::string_view);
  void 清空纹理池();
};
} // namespace 引擎::资源
