#pragma once

#include "spdlog/fmt/ostr.h"
#include "spdlog/spdlog.h"
#include <filesystem>

using Path = std::filesystem::path;

// 从 spdlog 输出 Path
template <> struct fmt::formatter<Path> : fmt::ostream_formatter {};
