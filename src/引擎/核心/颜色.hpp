#pragma once
#include "SDL3/SDL.h"
#include "glm/common.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/ext/vector_float4.hpp"
#include "glm/gtc/color_space.hpp" // 用于颜色空间转换
#include "imgui.h"
#include <format>
#include <string>

namespace 引擎::核心 {

struct 颜色 {
  glm::vec4 rgba{0.0f, 0.0f, 0.0f, 1.0f}; // 默认为黑色，不透明

  颜色() = default;
  explicit 颜色(SDL_Color 颜色值)
      : rgba(颜色值.r / 255.0f, 颜色值.g / 255.0f, 颜色值.b / 255.0f,
             颜色值.a / 255.0f) {}
  explicit 颜色(ImColor 颜色值)
      : rgba(颜色值.Value.x, 颜色值.Value.y, 颜色值.Value.z, 颜色值.Value.w) {}
  explicit 颜色(const glm::vec3 &颜色值)
      : rgba(颜色值.r, 颜色值.g, 颜色值.b, 1.0f) {}
  explicit 颜色(const glm::vec4 &颜色值) : rgba(颜色值) {}
  颜色(float r, float g, float b, float a = 1.0f) : rgba(r, g, b, a) {}
  颜色(int r, int g, int b, int a = 255)
      : rgba(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f) {}

  // 转换运算符
  operator glm::vec4() const { return rgba; }
  operator glm::vec3() const { return glm::vec3(rgba); }
  operator ImColor() const { return ImColor(rgba.r, rgba.g, rgba.b, rgba.a); }
  operator SDL_Color() const {
    return {static_cast<Uint8>(rgba.r * 255), static_cast<Uint8>(rgba.g * 255),
            static_cast<Uint8>(rgba.b * 255), static_cast<Uint8>(rgba.a * 255)};
  }

  // 使用GLM的向量运算
  颜色 operator+(const 颜色 &other) const { return 颜色(rgba + other.rgba); }

  颜色 operator-(const 颜色 &other) const { return 颜色(rgba - other.rgba); }

  颜色 operator*(const 颜色 &other) const { return 颜色(rgba * other.rgba); }

  颜色 operator*(float 标量) const { return 颜色(rgba * 标量); }

  颜色 operator/(const 颜色 &other) const { return 颜色(rgba / other.rgba); }

  颜色 operator/(float 标量) const { return 颜色(rgba / 标量); }

  // 复合赋值运算符
  颜色 &operator+=(const 颜色 &other) {
    rgba += other.rgba;
    return *this;
  }

  颜色 &operator-=(const 颜色 &other) {
    rgba -= other.rgba;
    return *this;
  }

  颜色 &operator*=(const 颜色 &other) {
    rgba *= other.rgba;
    return *this;
  }

  颜色 &operator*=(float 标量) {
    rgba *= 标量;
    return *this;
  }

  颜色 &operator/=(const 颜色 &other) {
    rgba /= other.rgba;
    return *this;
  }

  颜色 &operator/=(float 标量) {
    rgba /= 标量;
    return *this;
  }

  // 颜色空间转换
  颜色 转换为线性空间() const { return 颜色(glm::convertLinearToSRGB(rgba)); }

  颜色 转换为SRGB空间() const { return 颜色(glm::convertSRGBToLinear(rgba)); }

  // 钳制颜色值到[0,1]范围
  颜色 钳制() const { return 颜色(glm::clamp(rgba, 0.0f, 1.0f)); }

  // 获取颜色分量
  float &r() { return rgba.r; }
  float &g() { return rgba.g; }
  float &b() { return rgba.b; }
  float &a() { return rgba.a; }
  const float &r() const { return rgba.r; }
  const float &g() const { return rgba.g; }
  const float &b() const { return rgba.b; }
  const float &a() const { return rgba.a; }

  // 格式化输出
  std::string toString() const {
    return std::format("颜色 (r={:.2f}, g={:.2f}, b={:.2f},a={:.2f})", rgba.r,
                       rgba.g, rgba.b, rgba.a);
  }

  /// 默认色
  static 颜色 浅灰() { return {200, 200, 200, 255}; }
  static 颜色 灰() { return {130, 130, 130, 255}; }
  static 颜色 深灰() { return {80, 80, 80, 255}; }
  static 颜色 黄() { return {253, 249, 0, 255}; }
  static 颜色 金() { return {255, 203, 0, 255}; }
  static 颜色 橙() { return {255, 161, 0, 255}; }
  static 颜色 粉() { return {255, 109, 194, 255}; }
  static 颜色 红() { return {230, 41, 55, 255}; }
  static 颜色 栗() { return {190, 33, 55, 255}; }
  static 颜色 绿() { return {0, 228, 48, 255}; }
  static 颜色 柠檬绿() { return {0, 158, 47, 255}; }
  static 颜色 深绿() { return {0, 117, 44, 255}; }
  static 颜色 天蓝() { return {102, 191, 255, 255}; }
  static 颜色 蓝() { return {0, 121, 241, 255}; }
  static 颜色 深蓝() { return {0, 82, 172, 255}; }
  static 颜色 紫() { return {200, 122, 255, 255}; }
  static 颜色 堇紫() { return {135, 60, 190, 255}; }
  static 颜色 深紫() { return {112, 31, 126, 255}; }
  static 颜色 米黄() { return {211, 176, 131, 255}; }
  static 颜色 褐() { return {127, 106, 79, 255}; }
  static 颜色 深褐() { return {76, 63, 47, 255}; }
  static 颜色 白() { return {255, 255, 255, 255}; }
  static 颜色 黑() { return {0, 0, 0, 255}; }
  static 颜色 空白() { return {0, 0, 0, 0}; }
  static 颜色 红紫() { return {255, 0, 255, 255}; }
  static 颜色 白烟() { return {245, 245, 245, 255}; }
  /// 增加中国传统颜色 https://colors.ichuantong.cn/
  /// 金银
  static 颜色 赤金() { return {242, 190, 69}; }  /// HEX: #f2be45
  static 颜色 金色() { return {234, 205, 118}; } /// HEX: #eacd76
  static 颜色 银白() { return {233, 231, 239}; } /// HEX: #e9e7ef
  static 颜色 老银() { return {186, 202, 198}; } /// HEX: #bacac6
  static 颜色 乌金() { return {167, 142, 68}; }  /// HEX: #a78e44
  static 颜色 铜绿() { return {84, 150, 136}; }  /// HEX: #549688
  /// 黑
  static 颜色 玄色() { return {98, 42, 29}; }    /// HEX: #622a1d
  static 颜色 玄青() { return {61, 59, 79}; }    /// HEX: #3d3b4f
  static 颜色 乌色() { return {114, 94, 130}; }  /// HEX: #725e82
  static 颜色 乌黑() { return {57, 47, 65}; }    /// HEX: #392f41
  static 颜色 漆黑() { return {22, 24, 35}; }    /// HEX: #161823
  static 颜色 墨色() { return {80, 97, 109}; }   /// HEX: #50616d
  static 颜色 墨灰() { return {117, 138, 153}; } /// HEX: #758a99
  static 颜色 缁色() { return {73, 49, 49}; }    /// HEX: #493131 zi色
  static 颜色 煤黑() { return {49, 37, 32}; }    /// HEX: #312520
  static 颜色 黧() { return {93, 81, 60}; }      /// HEX: #5d513c li色
  static 颜色 黎() { return {117, 102, 77}; }    /// HEX: #75664d
  static 颜色 黝() { return {107, 104, 130}; }   /// HEX: #6b6882
  static 颜色 黝黑() { return {107, 87, 87}; }   /// HEX: #665757
  static 颜色 黯() { return {65, 85, 93}; }      /// HEX: #41555d
  /// 灰白
  static 颜色 精白() { return {255, 255, 255}; }   /// HEX: #ffffff
  static 颜色 象牙白() { return {255, 251, 240}; } /// HEX: #fffbf0
  static 颜色 雪白() { return {242, 253, 255}; }   /// HEX: #f2fdff
  static 颜色 月白() { return {214, 236, 240}; }   /// HEX: #d6ecf0
  static 颜色 缟() { return {242, 236, 222}; }     /// HEX: #f2ecde
  static 颜色 素() { return {224, 240, 233}; }     /// HEX: #e0f0e9
  static 颜色 茶白() { return {243, 249, 241}; }   /// HEX: #f3f9f1
  static 颜色 霜色() { return {233, 241, 246}; }   /// HEX: #e9f1f6
  static 颜色 花白() { return {194, 204, 208}; }   /// HEX: #c2ccd0
  static 颜色 鱼肚白() { return {252, 239, 232}; } /// HEX: #fcefe8
  static 颜色 荧白() { return {227, 249, 253}; }   /// HEX: #e3f9fd
  static 颜色 灰色() { return {128, 128, 128}; }   /// HEX: #808080
  static 颜色 牙色() { return {238, 222, 176}; }   /// HEX: #eedeb0
  static 颜色 铅白() { return {240, 240, 244}; }   /// HEX: #f0f0f4
  /// 水
  static 颜色 水色() { return {136, 173, 166}; } /// HEX: #88ada6
  static 颜色 水红() { return {243, 211, 231}; } /// HEX: #f3d3e7
  static 颜色 水绿() { return {212, 242, 231}; } /// HEX: #d4f2e7
  static 颜色 水蓝() { return {210, 240, 244}; } /// HEX: #d2f0f4
  static 颜色 淡青() { return {211, 224, 243}; } /// HEX: #d3e0f3
  static 颜色 湖蓝() { return {48, 223, 243}; }  /// HEX: #30dff3
  static 颜色 湖绿() { return {37, 248, 203}; }  /// HEX: #25f8cb
  /// 苍
  static 颜色 苍色() { return {117, 135, 138}; } /// HEX: #75878a
  static 颜色 苍翠() { return {81, 154, 115}; }  /// HEX: #519a73
  static 颜色 苍黄() { return {162, 155, 124}; } /// HEX: #a29b7c
  static 颜色 苍青() { return {115, 151, 171}; } /// HEX: #7397ab
  static 颜色 苍黑() { return {57, 82, 96}; }    /// HEX: #395260
  static 颜色 苍白() { return {209, 217, 224}; } /// HEX: #d1d9e0
  /// 蓝
  static 颜色 蓝色() { return {68, 206, 246}; }    /// HEX: #44cef6
  static 颜色 靛青() { return {23, 124, 176}; }    /// HEX: #177cb0
  static 颜色 靛蓝() { return {6, 82, 121}; }      /// HEX: #065279
  static 颜色 碧蓝() { return {62, 237, 231}; }    /// HEX: #3eede7
  static 颜色 蔚蓝() { return {112, 243, 255}; }   /// HEX: #70f3ff
  static 颜色 宝蓝() { return {75, 92, 196}; }     /// HEX: #4b5cc4
  static 颜色 蓝灰() { return {161, 175, 201}; }   /// HEX: #a1afc9
  static 颜色 藏青() { return {46, 78, 126}; }     /// HEX: #2e4e7e
  static 颜色 藏蓝() { return {59, 46, 126}; }     /// HEX: #3b2e7e
  static 颜色 黛() { return {74, 66, 102}; }       /// HEX: #4a4266
  static 颜色 黛绿() { return {66, 102, 102}; }    /// HEX: #426666
  static 颜色 黛蓝() { return {66, 80, 102}; }     /// HEX: #425066
  static 颜色 黛紫() { return {87, 66, 102}; }     /// HEX: #574266
  static 颜色 紫色() { return {141, 75, 187}; }    /// HEX: #8d4bbb
  static 颜色 紫酱() { return {129, 84, 99}; }     /// HEX: #815463
  static 颜色 酱紫() { return {129, 84, 118}; }    /// HEX: #815476
  static 颜色 紫檀() { return {76, 34, 27}; }      /// HEX: #4c221b
  static 颜色 绀青绀紫() { return {0, 51, 113}; }  /// HEX: #003371
  static 颜色 紫棠() { return {86, 0, 79}; }       /// HEX: #56004f
  static 颜色 青莲() { return {128, 29, 174}; }    /// HEX: #801dae
  static 颜色 群青() { return {76, 141, 174}; }    /// HEX: #4c8dae
  static 颜色 雪青() { return {176, 164, 227}; }   /// HEX: #b0a4e3
  static 颜色 丁香色() { return {204, 164, 227}; } /// HEX: #cca4e3
  static 颜色 藕色() { return {237, 209, 216}; }   /// HEX: #edd1d8
  static 颜色 藕荷色() { return {228, 198, 208}; } /// HEX: #e4c6d0
  /// 绿
  static 颜色 嫩绿() { return {189, 221, 34}; }    /// HEX: #bddd22
  static 颜色 柳黄() { return {201, 221, 34}; }    /// HEX: #c9dd22
  static 颜色 柳绿() { return {175, 221, 34}; }    /// HEX: #afdd22
  static 颜色 竹青() { return {120, 146, 98}; }    /// HEX: #789262
  static 颜色 葱黄() { return {163, 217, 0}; }     /// HEX: #a3d900
  static 颜色 葱绿() { return {158, 217, 0}; }     /// HEX: #9ed900
  static 颜色 葱青() { return {14, 184, 58}; }     /// HEX: #0eb83a
  static 颜色 葱倩() { return {14, 184, 64}; }     /// HEX: #0eb840
  static 颜色 青葱() { return {10, 163, 68}; }     /// HEX: #0aa344
  static 颜色 油绿() { return {0, 188, 18}; }      /// HEX: #00bc12
  static 颜色 绿沈() { return {12, 137, 24}; }     /// HEX: #0c8918
  static 颜色 碧色() { return {27, 209, 165}; }    /// HEX: #1bd1a5
  static 颜色 碧绿() { return {42, 221, 156}; }    /// HEX: #2add9c
  static 颜色 青碧() { return {72, 192, 163}; }    /// HEX: #48c0a3
  static 颜色 翡翠色() { return {61, 225, 173}; }  /// HEX: #3de1ad
  static 颜色 草绿() { return {64, 222, 90}; }     /// HEX: #40de5a
  static 颜色 青色() { return {0, 224, 158}; }     /// HEX: #00e09e
  static 颜色 青翠() { return {0, 224, 121}; }     /// HEX: #00e079
  static 颜色 青白() { return {192, 235, 215}; }   /// HEX: #c0ebd7
  static 颜色 鸭卵青() { return {224, 238, 232}; } /// HEX: #e0eee8
  static 颜色 蟹壳青() { return {187, 205, 197}; } /// HEX: #bbcdc5
  static 颜色 鸦青() { return {66, 76, 80}; }      /// HEX: #424c50
  static 颜色 绿色() { return {0, 229, 0}; }       /// HEX: #00e500
  static 颜色 豆绿() { return {158, 208, 72}; }    /// HEX: #9ed048
  static 颜色 豆青() { return {150, 206, 84}; }    /// HEX: #96ce54
  static 颜色 石青() { return {150, 206, 84}; }    /// HEX: #7bcfa6
  static 颜色 玉色() { return {46, 223, 163}; }    /// HEX: #2edfa3
  static 颜色 缥() { return {127, 236, 173}; }     /// HEX: #7fecad
  static 颜色 艾绿() { return {164, 226, 198}; }   /// HEX: #a4e2c6
  static 颜色 松柏绿() { return {33, 166, 117}; }  /// HEX: #21a675
  static 颜色 松花绿() { return {5, 119, 72}; }    /// HEX: #057748
  static 颜色 松花色() { return {188, 230, 114}; } /// HEX: #bce672
  /// 黄
  static 颜色 鹅黄() { return {255, 241, 67}; }   /// HEX: #fff143
  static 颜色 鸭黄() { return {250, 255, 114}; }  /// HEX: #faff72
  static 颜色 樱草色() { return {234, 255, 86}; } /// HEX: #eaff56
  static 颜色 杏黄() { return {255, 166, 49}; }   /// HEX: #ffa631
  static 颜色 杏红() { return {255, 140, 49}; }   /// HEX: #ff8c31
  static 颜色 橘黄() { return {255, 137, 54}; }   /// HEX: #ff8936
  static 颜色 橙黄() { return {255, 164, 0}; }    /// HEX: #ffa400
  static 颜色 橘红() { return {255, 117, 0}; }    /// HEX: #ff7500
  static 颜色 姜黄() { return {255, 199, 115}; }  /// HEX: #ffc773
  static 颜色 缃色() { return {240, 194, 57}; }   /// HEX: #f0c239
  static 颜色 橙色() { return {250, 140, 53}; }   /// HEX: #fa8c35
  static 颜色 茶色() { return {179, 92, 68}; }    /// HEX: #b35c44
  static 颜色 驼色() { return {168, 132, 98}; }   /// HEX: #a88462
  static 颜色 昏黄() { return {200, 155, 64}; }   /// HEX: #c89b40
  static 颜色 栗色() { return {96, 40, 30}; }     /// HEX: #60281e
  static 颜色 棕色() { return {178, 93, 37}; }    /// HEX: #b25d25
  static 颜色 棕绿() { return {130, 113, 0}; }    /// HEX: #827100
  static 颜色 棕黑() { return {124, 75, 0}; }     /// HEX: #7c4b00
  static 颜色 棕红() { return {155, 68, 0}; }     /// HEX: #9b4400
  static 颜色 棕黄() { return {174, 112, 0}; }    /// HEX: #ae7000
  static 颜色 赭() { return {156, 83, 51}; }      /// HEX: #9c5333
  static 颜色 赭色() { return {149, 85, 57}; }    /// HEX: #955539
  static 颜色 琥珀() { return {202, 105, 36}; }   /// HEX: #ca6924
  static 颜色 褐色() { return {110, 81, 30}; }    /// HEX: #6e511e
  static 颜色 枯黄() { return {211, 177, 125}; }  /// HEX: #d3b17d
  static 颜色 黄栌() { return {226, 156, 69}; }   /// HEX: #e29c45
  static 颜色 秋色() { return {137, 108, 57}; }   /// HEX: #896c39
  static 颜色 秋香色() { return {217, 182, 17}; } /// HEX: #d9b611
  /// 红
  static 颜色 粉红() { return {255, 179, 167}; }  /// HEX: #ffb3a7
  static 颜色 妃色() { return {37, 87, 54}; }     /// HEX: #ed5736
  static 颜色 品红() { return {240, 0, 86}; }     /// HEX: #f00056
  static 颜色 桃红() { return {244, 121, 131}; }  /// HEX: #f47983
  static 颜色 海棠红() { return {219, 90, 107}; } /// HEX: #db5a6b
  static 颜色 石榴红() { return {242, 12, 0}; }   /// HEX: #f20c00
  static 颜色 樱桃色() { return {201, 55, 86}; }  /// HEX: #c93756
  static 颜色 银红() { return {240, 86, 84}; }    /// HEX: #f05654
  static 颜色 大红() { return {255, 33, 33}; }    /// HEX: #ff2121
  static 颜色 绛紫() { return {140, 67, 86}; }    /// HEX: #8c4356
  static 颜色 绯红() { return {200, 60, 35}; }    /// HEX: #c83c23
  static 颜色 胭脂() { return {157, 41, 51}; }    /// HEX: #9d2933
  static 颜色 朱红() { return {255, 76, 0}; }     /// HEX: #ff4c00
  static 颜色 丹() { return {255, 78, 32}; }      /// HEX: #ff4e20
  static 颜色 彤() { return {243, 83, 54}; }      /// HEX: #f35336
  static 颜色 茜色() { return {203, 58, 86}; }    /// HEX: #cb3a56
  static 颜色 火红() { return {255, 45, 81}; }    /// HEX: #ff2d51
  static 颜色 赫赤() { return {201, 31, 55}; }    /// HEX: #c91f37
  static 颜色 嫣红() { return {239, 122, 130}; }  /// HEX: #ef7a82
  static 颜色 洋红() { return {255, 0, 151}; }    /// HEX: #ff0097
  static 颜色 炎() { return {255, 51, 0}; }       /// HEX: #ff3300
  static 颜色 赤() { return {195, 39, 43}; }      /// HEX: #c3272b
  static 颜色 绾() { return {169, 129, 117}; }    /// HEX: #a98175
  static 颜色 枣红() { return {195, 33, 54}; }    /// HEX: #c32136
  static 颜色 檀() { return {179, 109, 97}; }     /// HEX: #b36d61
  static 颜色 殷红() { return {190, 0, 47}; }     /// HEX: #be002f
  static 颜色 酡红() { return {220, 48, 35}; }    /// HEX: #dc3023
  static 颜色 酡颜() { return {249, 144, 111}; }  /// HEX: #f9906f
};

} // namespace 引擎::核心