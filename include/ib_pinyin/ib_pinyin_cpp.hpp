#pragma once

#include "ib_pinyin/ib_pinyin.h"
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

namespace ib {

class 拼音匹配器 {
public:
  // 拼音表示法选项
  enum class 拼音方案 : uint32_t {
    // 基础表示法
    简拼 = 0x1,        ///< 简拼，如 "p", "y"
    全拼 = 0x2,        ///< 全拼，如 "pin", "yin"
    全拼声调 = 0x4,    ///< 带声调全拼，如 "pin1", "yin1"
    Unicode声调 = 0x8, ///< Unicode 声调，如 "pīn", "yīn"

    // 双拼方案
    智能ABC双拼 = 0x10,  ///< 智能 ABC 双拼
    拼音加加双拼 = 0x20, ///< 拼音加加双拼
    微软双拼 = 0x40,     ///< 微软双拼
    紫光双拼 = 0x80,     ///< 华宇双拼（紫光双拼）
    小鹤双拼 = 0x100,    ///< 小鹤双拼
    自然码双拼 = 0x200,  ///< 自然码双拼

    // 常用组合
    默认 = 全拼 | 简拼, ///< 默认使用全拼
    全部 = 0x3FF        ///< 所有表示法的组合
  };

  // 支持按位或操作组合多个表示法
  friend 拼音方案 operator|(拼音方案 a, 拼音方案 b) {
    return static_cast<拼音方案>(static_cast<uint32_t>(a) |
                                 static_cast<uint32_t>(b));
  }

  // 构造函数
  拼音匹配器() : ptr_(nullptr) {
    // 如果需要初始化，在这里添加
  }

  // 析构函数
  ~拼音匹配器() {
    if (ptr_) {
      capi::ib_pinyin_destroy(ptr_);
    }
  }

  // 禁用拷贝构造和赋值
  拼音匹配器(const 拼音匹配器 &) = delete;
  拼音匹配器 &operator=(const 拼音匹配器 &) = delete;

  // 移动构造和赋值
  拼音匹配器(拼音匹配器 &&other) noexcept : ptr_(other.ptr_) {
    other.ptr_ = nullptr;
  }

  拼音匹配器 &operator=(拼音匹配器 &&other) noexcept {
    if (this != &other) {
      if (ptr_) {
        capi::ib_pinyin_destroy(ptr_);
      }
      ptr_ = other.ptr_;
      other.ptr_ = nullptr;
    }
    return *this;
  }

  // UTF-8 字符串匹配
  bool 是否匹配(std::string 拼音, std::string 匹配项,
                拼音方案 方案 = 拼音方案::默认) const {
    return capi::ib_pinyin_is_match_u8(拼音.data(), 拼音.length(),
                                       匹配项.data(), 匹配项.length(),
                                       static_cast<uint32_t>(方案));
  }

  // UTF-16 字符串匹配
  bool 是否匹配(std::u16string 拼音, std::u16string 匹配项,
                拼音方案 方案 = 拼音方案::默认) const {
    return capi::ib_pinyin_is_match_u16(
        reinterpret_cast<const uint16_t *>(拼音.data()), 拼音.length(),
        reinterpret_cast<const uint16_t *>(匹配项.data()), 匹配项.length(),
        static_cast<uint32_t>(方案));
  }

  // UTF-32 字符串匹配
  bool 是否匹配(std::u32string 拼音, std::u32string 匹配项,
                拼音方案 方案 = 拼音方案::默认) const {
    return capi::ib_pinyin_is_match_u32(
        reinterpret_cast<const uint32_t *>(拼音.data()), 拼音.length(),
        reinterpret_cast<const uint32_t *>(匹配项.data()), 匹配项.length(),
        static_cast<uint32_t>(方案));
  }

  // UTF-8 字符串查找匹配位置
  size_t 查找匹配位置(std::string 拼音, std::string 匹配项,
                      拼音方案 notation = 拼音方案::默认) const {
    return capi::ib_pinyin_find_match_u8(拼音.data(), 拼音.length(),
                                         匹配项.data(), 匹配项.length(),
                                         static_cast<uint32_t>(notation));
  }

  // UTF-16 字符串查找匹配位置
  size_t 查找匹配位置(std::u16string 拼音, std::u16string 匹配项,
                      拼音方案 方案 = 拼音方案::默认) const {
    return capi::ib_pinyin_find_match_u16(
        reinterpret_cast<const uint16_t *>(拼音.data()), 拼音.length(),
        reinterpret_cast<const uint16_t *>(匹配项.data()), 匹配项.length(),
        static_cast<uint32_t>(方案));
  }

  // UTF-32 字符串查找匹配位置
  size_t 查找匹配位置(std::u32string 拼音, std::u32string 匹配项,
                      拼音方案 方案 = 拼音方案::默认) const {
    return capi::ib_pinyin_find_match_u32(
        reinterpret_cast<const uint32_t *>(拼音.data()), 拼音.length(),
        reinterpret_cast<const uint32_t *>(匹配项.data()), 匹配项.length(),
        static_cast<uint32_t>(方案));
  }

private:
  capi::ib_pinyin *ptr_;
};

} // namespace ib