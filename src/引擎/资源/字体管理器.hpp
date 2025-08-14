#pragma once
#include <SDL3_ttf/SDL_ttf.h> // SDL_ttf 主头文件
#include <functional>         // 用于 std::hash
#include <memory>             // 用于 std::unique_ptr
#include <stdexcept>          // 用于 std::runtime_error
#include <string>             // 用于 std::string
#include <string_view>        // 用于 std::string_view
#include <unordered_map>      // 用于 std::unordered_map
#include <utility>            // 用于 std::pair

namespace 引擎::资源 {

// 定义字体键类型（路径 + 大小）
using FontKey = std::pair<std::string, int>;
// std::pair是标准库中的一个类模板，用于将两个值组合成一个单元

// FontKey 的自定义哈希函数（std::pair<std::string, int>），用于
// std::unordered_map
struct FontKeyHash {
  std::size_t operator()(const FontKey &key) const {
    std::hash<std::string> string_hasher;
    std::hash<int> int_hasher;
    return string_hasher(key.first) ^
           int_hasher(
               key.second); // 异或运算符 ^
                            // 按位计算，每一位的两个值不同为1，相同为0，这是合并两个哈希值的简单方法
  }
};

/**
 * @brief 管理 SDL_ttf 字体资源（TTF_Font）。
 *
 * 提供字体的加载和缓存功能，通过文件路径和点大小来标识。
 * 构造失败会抛出异常。仅供 ResourceManager 内部使用。
 */
class 字体管理器 final {
  friend class 资源管理器;

private:
  // TTF_Font 的自定义删除器
  struct SDLFont删除器 {
    void operator()(TTF_Font *font) const {
      if (font) {
        TTF_CloseFont(font);
      }
    }
  };

  // 字体存储（FontKey -> TTF_Font）。
  // unordered_map 的键需要能转换为哈希值，对于基础数据类型，系统会自动转换
  // 但是对于对于自定义类型（系统无法自动转化），则需要提供自定义哈希函数（第三个模版参数）
  std::unordered_map<FontKey, std::unique_ptr<TTF_Font, SDLFont删除器>,
                     FontKeyHash>
      _字体池;

public:
  /**
   * @brief 构造函数。初始化 SDL_ttf。
   * @throws std::runtime_error 如果 SDL_ttf 初始化失败。
   */
  字体管理器();

  ~字体管理器(); // 需要手动添加析构函数，清理资源并关闭 SDL_ttf。

  // 当前设计中，我们只需要一个FontManager，所有权不变，所以不需要拷贝、移动相关构造及赋值运算符
  字体管理器(const 字体管理器 &) = delete;
  字体管理器 &operator=(const 字体管理器 &) = delete;
  字体管理器(字体管理器 &&) = delete;
  字体管理器 &operator=(字体管理器 &&) = delete;

private: // 仅由 ResourceManager（和内部）访问的方法
  //  从文件路径加载指定点大小的字体
  TTF_Font *载入字体(std::string_view, int 字号大小);
  // 尝试获取已加载字体的指针，如果未加载则尝试加载
  TTF_Font *获取字体(std::string_view, int 字号大小);
  // 卸载特定字体（通过路径和大小标识）
  void 卸载字体(std::string_view, int 字号大小);

  void 清空字体池(); // 清空所有缓存的字体
};

} // namespace 引擎::资源