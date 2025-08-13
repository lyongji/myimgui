#pragma once

#include "Path.hpp"
#include "UUID.hpp"

namespace 引擎::资源 {

struct 资源加载结果 {
  核心::UUID 资源UUID;
  Path 资源路径;

  operator bool() const { return static_cast<bool>(资源UUID); }
};
} // namespace 引擎::资源