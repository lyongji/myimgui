#include "glm/ext/vector_float2.hpp"
#include <string_view>
// #include "glm/glm.hpp"
#include <memory>

// 前向声明 SDL 类型
struct SDL_Renderer;
struct SDL_Texture;
struct Mix_Chunk;
struct Mix_Music;
struct TTF_Font;

namespace 引擎::资源 {
// 前向声明内部管理器
class 纹理管理器;
class 音频管理器;
class 字体管理器;

/**
 * @brief 作为访问各种资源管理器的中央控制点（外观模式 Facade）。
 * 在构造时初始化其管理的子系统。构造失败会抛出异常。
 */
class 资源管理器 {
private:
  std::unique_ptr<纹理管理器> _纹理管理器;
  std::unique_ptr<音频管理器> _音频管理器;
  std::unique_ptr<字体管理器> _字体管理器;

public:
  // 构造函数
  explicit 资源管理器(SDL_Renderer *渲染器);
  // 析构函数
  ~资源管理器();

  void 清空资源(); // 清空所有资源

  资源管理器(const 资源管理器 &) = delete;
  资源管理器 &operator=(const 资源管理器 &) = delete;
  资源管理器(资源管理器 &&) = delete;
  资源管理器 &operator=(资源管理器 &&) = delete;

  // 统一资源访问

  // 获取纹理
  SDL_Texture *载入纹理(std::string_view 文件路径);
  SDL_Texture *获取纹理(std::string_view 文件路径);
  void 卸载纹理(std::string_view 文件路径);
  glm::vec2 获取纹理尺寸(std::string_view 文件路径);
  void 清空纹理池();

  // 获取音效
  Mix_Chunk *载入音效(std::string_view 文件路径);
  Mix_Chunk *获取音效(std::string_view 文件路径);
  void 卸载音效(std::string_view 文件路径);
  void 清空音效池();

  // 获取音乐
  Mix_Music *载入音乐(std::string_view 文件路径);
  Mix_Music *获取音乐(std::string_view 文件路径);
  void 卸载音乐(std::string_view 文件路径);
  void 清空音乐池();

  // 获取字体
  TTF_Font *载入字体(std::string_view 文件路径, int 字体大小);
  TTF_Font *获取字体(std::string_view 文件路径, int 字体大小);
  void 卸载字体(std::string_view 文件路径, int 字体大小);
  void 清空字体池();
};

} // namespace 引擎::资源