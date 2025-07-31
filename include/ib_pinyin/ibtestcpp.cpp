#include "ib_pinyin/ib_pinyin_cpp.hpp"
#include "print"

int main() {
  ib::拼音匹配器 匹配器;

  std::string 拼音 = "zgr";
  std::string 文本 = "中国人民";

  // 使用默认全拼/简拼匹配
  std::println("{}", 匹配器.是否匹配(拼音, 文本));
  if (匹配器.是否匹配(拼音, 文本)) {
    std::println("全拼匹配成功");
  }

  // 组合使用多种表示法

  auto 方案 =
      ib::拼音匹配器::拼音方案::自然码双拼 | ib::拼音匹配器::拼音方案::简拼;

  std::println("{}", 匹配器.是否匹配(拼音, 文本));
  if (匹配器.是否匹配(拼音, 文本, 方案)) {
    std::println("自然码双拼匹配成功");
  }

  // 使用双拼方案
  if (匹配器.是否匹配(拼音, 文本, ib::拼音匹配器::拼音方案::微软双拼)) {
    std::println("微软双拼匹配成功");
  }

  return 0;
}
