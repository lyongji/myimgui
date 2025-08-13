#pragma once
/*
   UUID‑v7 生成器 – RFC 9562 §5.7
   ----------------------------------
   * 头文件形式，进程全局，无锁UUID v7生成
   * 单调性：48位毫秒时间戳 + 12位序列，自然可排序
   * 每毫秒使用新熵重新播种12位序列
   * **漂移上限**：虚拟时钟最多可领先系统时钟1000毫秒
   * **突发限制**：每毫秒最多生成4095个UUID，之后推进虚拟时间
   * 首个UUID从随机非零序列开始（保持排序顺序）
   * 64位原子操作必须无锁（编译时检查）
   * 使用std::random_device + 时序作为熵源 - 非加密安全
   * CAS失败后采用指数退避
   * 时钟回滚保护：暂停直到系统时间赶上
*/
#include <array>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <memory>
#include <optional>
#include <random>
#include <string>
#include <thread>

namespace 引擎::核心 {

static_assert(std::atomic<std::uint64_t>::is_always_lock_free,
              "64位原子操作必须无锁，用于UUIDv7生成器");
// uuid v7生成器
struct UUID {
  std::array<std::uint8_t, 16> 字节数组{};

  // 默认构造函数：初始化为全零（空UUID）
  UUID() noexcept { 字节数组.fill(0); }
  // 静态函数：生成新的 UUID v7
  static UUID 生成();
  // 静态函数：生成指定数量的 UUID v7 ,有序列表
  static std::vector<UUID> 批量生成(int count);
  // 显式 bool 转换运算符：检查 UUID 是否非零（非空）
  explicit operator bool() const noexcept {
    for (const auto &字节 : 字节数组) {
      if (字节 != 0)
        return true;
    }
    return false;
  }

  // 检查是否为空（全零）UUID
  [[nodiscard]] bool 是否为空() const noexcept {
    return !static_cast<bool>(*this);
  }
  [[nodiscard]] std::string str() const {
    static constexpr char 十六进制字符[] = "0123456789abcdef";
    // 36字符字符串中每个字节的位置（考虑连字符）
    static constexpr int 位置[16] = {0,  2,  4,  6,  9,  11, 14, 16,
                                     19, 21, 24, 26, 28, 30, 32, 34};

    std::array<char, 36> 缓冲区{};
    缓冲区[8] = 缓冲区[13] = 缓冲区[18] = 缓冲区[23] = '-';

    for (int 索引 = 0; 索引 < 16; ++索引) {
      unsigned 字节值 = 字节数组[索引];
      缓冲区[位置[索引]] = 十六进制字符[字节值 >> 4];
      缓冲区[位置[索引] + 1] = 十六进制字符[字节值 & 0x0F];
    }

    return {缓冲区.data(), 缓冲区.size()};
  }

  // 获取时间戳部分（毫秒）
  [[nodiscard]] std::uint64_t 时间戳() const {
    std::uint64_t timestamp = 0;
    for (int i = 0; i < 6; ++i) {
      timestamp = (timestamp << 8) | 字节数组[i];
    }
    return timestamp;
  }

  // 默认严格比较（全部16字节）
  bool operator==(const UUID &other) const = default;
  auto operator<=>(const UUID &other) const = default;
};
// 重载 << 运算符以便输出UUID
inline std::ostream &operator<<(std::ostream &os, const UUID &uuid) {
  return os << uuid.str();
}

//// UUIDv7生成器
class UUIDv7生成器 {
public:
  UUIDv7生成器() = default;
  ~UUIDv7生成器() = default;

  // 不可复制：所有实例共享全局状态
  UUIDv7生成器(const UUIDv7生成器 &) = delete;
  UUIDv7生成器(UUIDv7生成器 &&) = delete;
  UUIDv7生成器 &operator=(const UUIDv7生成器 &) = delete;
  UUIDv7生成器 &operator=(UUIDv7生成器 &&) = delete;

  static constexpr std::uint16_t 序列掩码常量 = 0x0FFF; // 12位序列 (0-4095)

  // 返回UUID或空值（如果漂移/突发限制超出）
  [[nodiscard]] std::optional<UUID> 生成UUID();

  // 诊断：虚拟时钟漂移毫秒数（+ = 领先，- = 落后）
  [[nodiscard]] static std::int64_t 获取当前漂移毫秒() {
    auto 实际时间毫秒 = 计算自纪元毫秒数(std::chrono::system_clock::now());
    auto 状态时间毫秒 = 全局状态.load(std::memory_order_acquire) >> 16;
    return static_cast<std::int64_t>(状态时间毫秒) -
           static_cast<std::int64_t>(实际时间毫秒);
  }

private:
  static std::uint64_t
  计算自纪元毫秒数(std::chrono::system_clock::time_point tp) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               tp.time_since_epoch())
        .count();
  }

  // 调整参数
  static constexpr std::uint64_t 最大漂移毫秒常量 =
      1000;                                        // 相对于实际时间的最大漂移
  static constexpr unsigned 退避阈值常量 = 8;      // CAS失败后开始休眠的阈值
  static constexpr unsigned 初始退避微秒常量 = 1;  // 初始休眠时长
  static constexpr unsigned 最大退避微秒常量 = 64; // 最大休眠时长

  // 全局状态：[48位时间戳毫秒][16位序列]
  // 所有线程竞争更新此原子变量
  static std::atomic<std::uint64_t> 全局状态;

  // 每线程RNG以避免竞争
  struct 线程本地存储结构 {
    std::mt19937_64 随机引擎;
    std::uniform_int_distribution<std::uint16_t> 十二位分布{0, 序列掩码常量};
    std::uniform_int_distribution<std::uint16_t> 八位分布{
        0, 0xFF}; // 修复：使用uint16_t替代uint8_t

    线程本地存储结构() : 随机引擎(播种引擎()) {}

    // 使用多个熵源播种以确保线程唯一性
    static std::mt19937_64 播种引擎() {
      std::random_device rd;
      const auto 线程ID哈希值 =
          std::hash<std::thread::id>{}(std::this_thread::get_id());
      const auto 单调纳秒数 =
          std::chrono::steady_clock::now().time_since_epoch().count();
      return std::mt19937_64(rd() ^ 线程ID哈希值 ^
                             static_cast<std::uint64_t>(单调纳秒数));
    }
  };

  static thread_local 线程本地存储结构 线程本地存储;

  // 生成非零随机序列（保持字典顺序）
  static std::uint16_t 生成新序列() {
    std::uint16_t 序列值;
    do {
      序列值 = 线程本地存储.十二位分布(线程本地存储.随机引擎);
    } while (序列值 == 0);
    return 序列值;
  }

  static UUID 编码UUID(std::uint64_t 时间戳, std::uint16_t 序列);
};

inline std::optional<UUID> UUIDv7生成器::生成UUID() {
  using clock = std::chrono::system_clock;
  std::uint64_t 实际时间毫秒 = 计算自纪元毫秒数(clock::now());
  unsigned 失败计数 = 0;
  unsigned 退避微秒数 = 初始退避微秒常量;

  // 主CAS循环
  for (;;) {
    std::uint64_t 旧状态 = 全局状态.load(std::memory_order_acquire);
    std::uint64_t 旧时间戳 = 旧状态 >> 16;
    std::uint16_t 旧序列 = static_cast<std::uint16_t>(旧状态 & 0xFFFF);

    // 强制漂移上限：限制我们追赶实际时间的速度
    if (实际时间毫秒 > 旧时间戳 + 最大漂移毫秒常量)
      实际时间毫秒 = 旧时间戳 + 最大漂移毫秒常量;

    std::uint64_t 新时间戳 = 旧时间戳;
    std::uint16_t 新序列 = 旧序列;

    if (实际时间毫秒 > 旧时间戳) {
      // 时间前进：使用新时间戳和新随机序列
      新时间戳 = 实际时间毫秒;
      新序列 = 生成新序列();
    } else {
      // 同一毫秒：递增序列或处理回绕
      if (旧序列 == 0 && 实际时间毫秒 == 旧时间戳)
        新序列 = 生成新序列(); // 从零重置以保持顺序
      else
        新序列 = static_cast<std::uint16_t>((新序列 + 1) & 序列掩码常量);

      if (新序列 == 0) {
        // 序列回绕：虚拟时间前进1毫秒
        if (旧时间戳 + 1 > 实际时间毫秒 + 最大漂移毫秒常量)
          return std::nullopt; // 将超过漂移限制
        新时间戳 += 1;
        新序列 = 生成新序列();
      }
    }

    if (新时间戳 > 实际时间毫秒 + 最大漂移毫秒常量)
      return std::nullopt;

    // 尝试原子更新
    const std::uint64_t 新状态 = (新时间戳 << 16) | 新序列;
    if (全局状态.compare_exchange_weak(旧状态, 新状态,
                                       std::memory_order_acq_rel,
                                       std::memory_order_relaxed))
      return 编码UUID(新时间戳, 新序列);

    // CAS失败：指数退避以减少竞争
    if (++失败计数 >= 退避阈值常量) {
      std::this_thread::sleep_for(std::chrono::microseconds(退避微秒数));
      退避微秒数 = std::min(退避微秒数 * 2, 最大退避微秒常量);
      失败计数 = 0;
    }
    实际时间毫秒 = 计算自纪元毫秒数(clock::now());
  }
}

// 将时间戳和序列打包为RFC 9562 UUID v7格式
inline UUID UUIDv7生成器::编码UUID(std::uint64_t 时间戳, std::uint16_t 序列) {
  UUID UUID对象;
  auto *数据指针 = UUID对象.字节数组.data();

  // 48位大端时间戳（字节0-5）
  数据指针[0] = static_cast<std::uint8_t>((时间戳 >> 40) & 0xFF);
  数据指针[1] = static_cast<std::uint8_t>((时间戳 >> 32) & 0xFF);
  数据指针[2] = static_cast<std::uint8_t>((时间戳 >> 24) & 0xFF);
  数据指针[3] = static_cast<std::uint8_t>((时间戳 >> 16) & 0xFF);
  数据指针[4] = static_cast<std::uint8_t>((时间戳 >> 8) & 0xFF);
  数据指针[5] = static_cast<std::uint8_t>(时间戳 & 0xFF);

  // 版本(0111) + 序列高半字节（字节6）
  数据指针[6] = static_cast<std::uint8_t>(0x70 | ((序列 >> 8) & 0x0F));
  // 序列低字节（字节7）
  数据指针[7] = static_cast<std::uint8_t>(序列 & 0xFF);

  // 变体(10xx) + 6随机位（字节8）
  数据指针[8] = static_cast<std::uint8_t>(
      0x80 |
      (static_cast<std::uint8_t>(线程本地存储.八位分布(线程本地存储.随机引擎)) &
       0x3F));

  // 剩余56随机位（字节9-15）
  for (int 索引 = 9; 索引 < 16; ++索引)
    数据指针[索引] =
        static_cast<std::uint8_t>(线程本地存储.八位分布(线程本地存储.随机引擎));

  return UUID对象;
}

namespace detail {
// 为进程启动生成非零初始序列
inline std::uint16_t 生成初始序列() {
  std::random_device rd;
  const auto 单调时间 =
      std::chrono::steady_clock::now().time_since_epoch().count();
  std::uint16_t 序列值 = static_cast<std::uint16_t>((rd() ^ 单调时间) &
                                                    UUIDv7生成器::序列掩码常量);
  return 序列值 ? 序列值 : 1;
}
} // namespace detail

// 使用当前时间 + 随机序列初始化全局状态
inline std::atomic<std::uint64_t> UUIDv7生成器::全局状态{
    (static_cast<std::uint64_t>(
         std::chrono::duration_cast<std::chrono::milliseconds>(
             std::chrono::system_clock::now().time_since_epoch())
             .count())
     << 16) |
    detail::生成初始序列()};

inline UUID UUID::生成() {
  static UUIDv7生成器 生成器; // 静态生成器实例

  std::optional<UUID> uuid_opt;
  while (!(uuid_opt = 生成器.生成UUID())) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  return *uuid_opt;
}
inline std::vector<UUID> UUID::批量生成(int count) {
  std::vector<UUID> uuids;
  uuids.reserve(count);
  for (int i = 0; i < count; ++i) {
    uuids.push_back(生成());
  }
  return uuids;
}
inline thread_local UUIDv7生成器::线程本地存储结构 UUIDv7生成器::线程本地存储{};
} // namespace 引擎::核心

#if 0
// 头文件UUID-v7生成器的最小演示
#include <iostream>
#include <thread>

int main()
{
    UUIDv7生成器 生成器实例;
    
    for (int 计数器 = 0; 计数器 < 10; ++计数器)
    {
        // 尝试生成，使用非常简单的重试策略
        std::optional<UUIDv7> UUID对象;
        while (!(UUID对象 = 生成器实例.生成UUID()))
        {
            // 达到漂移/突发上限 — 退避1毫秒并重试
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        std::cout << UUID对象->ToString() << '\n';
    }
    
    // 显示当前虚拟与实际时钟的漂移（应该很小）
    std::cout << "当前漂移: " << UUIDv7生成器::获取当前漂移毫秒() << " 毫秒\n";
}
#endif

#if 0
// 使用N线程对头文件UUID-v7生成器进行压力测试
#include <chrono>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_set>
#include <vector>

//--------------------------------------------------------------------
// 工作线程：生成`每线程样本数`个UUID，并将它们附加到共享的
// `项目集合`向量中（使用互斥锁保护）
//--------------------------------------------------------------------
std::mutex 全局项互斥锁;

void 工作线程函数(std::vector<std::string>& 项目集合, int 每线程样本数)
{
    std::vector<std::string> 本地项目集合;
    本地项目集合.reserve(每线程样本数);
    
    static UUIDv7生成器 生成器实例;   // 共享状态位于头文件中；线程安全
    
    for (int 计数器 = 0; 计数器 < 每线程样本数; ++计数器)
    {
        std::optional<UUIDv7> UUID对象;
        while (!(UUID对象 = 生成器实例.生成UUID()))
        {
            // 达到漂移/突发上限 — 快速退避（内部退避已有所帮助）
            std::this_thread::sleep_for(std::chrono::microseconds(100));
        }
        本地项目集合.emplace_back(UUID对象->转换为字符串());
    }
    
    // 在锁保护下，将结果一次性移动到共享向量中
    {
        std::lock_guard<std::mutex> 锁(全局项互斥锁);
        项目集合.insert(项目集合.end(),
            std::make_move_iterator(本地项目集合.begin()),
            std::make_move_iterator(本地项目集合.end()));
    }
}

//--------------------------------------------------------------------
// 测试框架
//--------------------------------------------------------------------
int main()
{
    constexpr int 线程数量 = 16;
    constexpr int 每线程样本数 = 50000;   // 50k × 16 = 总计800k个UUID
    constexpr int 总项目数 = 线程数量 * 每线程样本数;
    
    std::vector<std::string> 项目集合;
    项目集合.reserve(总项目数);
    
    auto 开始时间 = std::chrono::high_resolution_clock::now();
    
    std::vector<std::thread> 线程集合;
    for (int 线程索引 = 0; 线程索引 < 线程数量; ++线程索引)
        线程集合.emplace_back(工作线程函数, std::ref(项目集合), 每线程样本数);
        
    for (auto& 线程 : 线程集合) 线程.join();
    
    auto 结束时间 = std::chrono::high_resolution_clock::now();
    auto 微秒数 = std::chrono::duration_cast<std::chrono::microseconds>(结束时间 - 开始时间).count();
    
    std::cout.setf(std::ios::fixed);
    std::cout << "生成 " << 总项目数 << " 个UUID耗时 " << 微秒数 << " 微秒 (" 
              << (总项目数 * 1'000'000.0 / 微秒数) << " UUID/秒)\n";
    
    // 唯一性检查
    std::unordered_set<std::string> 唯一集合(项目集合.begin(), 项目集合.end());
    std::cout << "唯一项目: " << 唯一集合.size() << "/" << 项目集合.size() << "\n";
    
    // 解码并打印前3个UUID以供视觉检查（可选）
    for (int 索引 = 0; 索引 < 3 && 索引 < (int)项目集合.size(); ++索引)
        std::cout << 项目集合[索引] << '\n';
}
#endif