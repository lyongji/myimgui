#include "字体管理器.hpp"
#include "日志.hpp"
#include <stdexcept>

namespace 引擎::资源 {

字体管理器::字体管理器() {
  if (!TTF_WasInit() && !TTF_Init()) {
    throw std::runtime_error("字体管理器 错误: TTF_Init 失败：" +
                             std::string(SDL_GetError()));
  }
  记录跟踪("字体管理器 构造成功。");
}

字体管理器::~字体管理器() {
  if (!_字体池.empty()) {
    记录调试("字体管理器 不为空，调用 清空字体池 处理清理逻辑。");
    清空字体池(); // 调用 清空字体池 处理清理逻辑
  }
  TTF_Quit();
  记录跟踪("字体管理器 析构成功。");
}

TTF_Font *字体管理器::载入字体(std::string_view 路径, int 字号大小) {
  // 检查点大小是否有效
  if (字号大小 <= 0) {
    记录错误("无法加载字体 '{}'：无效的点大小 {}。", 路径, 字号大小);
    return nullptr;
  }

  // 创建映射表的键
  FontKey key = {std::string(路径), 字号大小};

  // 首先检查缓存
  auto it = _字体池.find(key);
  if (it != _字体池.end()) {
    return it->second.get();
  }

  // 缓存中不存在，则加载字体
  记录调试("正在加载字体：{} ({}pt)", 路径, 字号大小);
  TTF_Font *原始字体 = TTF_OpenFont(路径.data(), 字号大小);
  if (!原始字体) {
    记录错误("加载字体 '{}' ({}pt) 失败：{}", 路径, 字号大小, SDL_GetError());
    return nullptr;
  }

  // 使用 unique_ptr 存储到缓存中
  _字体池.emplace(key, std::unique_ptr<TTF_Font, SDLFont删除器>(原始字体));
  记录调试("成功加载并缓存字体：{} ({}pt)", 路径, 字号大小);
  return 原始字体;
}

TTF_Font *字体管理器::获取字体(std::string_view 路径, int 字号大小) {
  FontKey key = {std::string(路径), 字号大小};
  auto it = _字体池.find(key);
  if (it != _字体池.end()) {
    return it->second.get();
  }

  spdlog::warn("字体 '{}' ({}pt) 不在缓存中，尝试加载。", 路径, 字号大小);
  return 载入字体(路径, 字号大小);
}

void 字体管理器::卸载字体(std::string_view 路径, int 字号大小) {
  FontKey key = {std::string(路径), 字号大小};
  auto it = _字体池.find(key);
  if (it != _字体池.end()) {
    记录调试("卸载字体：{} ({}pt)", 路径, 字号大小);
    _字体池.erase(it); // unique_ptr 会处理 TTF_CloseFont
  } else {
    spdlog::warn("尝试卸载不存在的字体：{} ({}pt)", 路径, 字号大小);
  }
}

void 字体管理器::清空字体池() {
  if (!_字体池.empty()) {
    记录调试("正在清理所有 {} 个缓存的字体。", _字体池.size());
    _字体池.clear(); // unique_ptr 会处理删除
  }
}

} // namespace 引擎::资源