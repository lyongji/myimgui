#pragma once

#include "spdlog/spdlog.h"
#include <memory>

class 日志管理器 {
public:
  static 日志管理器 &获取实例();

  auto &获取控制台日志() { return _控制台日志; }

  auto &获取文件日志() { return _文件日志; }

private:
  std::shared_ptr<spdlog::logger> _控制台日志;
  std::shared_ptr<spdlog::logger> _文件日志;

  日志管理器();

  static 日志管理器 管理器实例;
};

#define 记录信息(格式, ...)                                                    \
  do {                                                                         \
    SPDLOG_LOGGER_INFO(日志管理器::获取实例().获取控制台日志(), 格式,          \
                       ##__VA_ARGS__);                                         \
    SPDLOG_LOGGER_INFO(日志管理器::获取实例().获取文件日志(), 格式,            \
                       ##__VA_ARGS__);                                         \
  } while (0)

#define 记录错误(格式, ...)                                                    \
  do {                                                                         \
    SPDLOG_LOGGER_ERROR(日志管理器::获取实例().获取控制台日志(), 格式,         \
                        ##__VA_ARGS__);                                        \
    SPDLOG_LOGGER_ERROR(日志管理器::获取实例().获取文件日志(), 格式,           \
                        ##__VA_ARGS__);                                        \
  } while (0)

#define 记录警告(格式, ...)                                                    \
  do {                                                                         \
    SPDLOG_LOGGER_WARN(日志管理器::获取实例().获取控制台日志(), 格式,          \
                       ##__VA_ARGS__);                                         \
    SPDLOG_LOGGER_WARN(日志管理器::获取实例().获取文件日志(), 格式,            \
                       ##__VA_ARGS__);                                         \
  } while (0)

#define 记录调试(格式, ...)                                                    \
  do {                                                                         \
    SPDLOG_LOGGER_DEBUG(日志管理器::获取实例().获取控制台日志(), 格式,         \
                        ##__VA_ARGS__);                                        \
    SPDLOG_LOGGER_DEBUG(日志管理器::获取实例().获取文件日志(), 格式,           \
                        ##__VA_ARGS__);                                        \
  } while (0)

#define 记录关键(格式, ...)                                                    \
  do {                                                                         \
    SPDLOG_LOGGER_CRITICAL(日志管理器::获取实例().获取控制台日志(), 格式,      \
                           ##__VA_ARGS__);                                     \
    SPDLOG_LOGGER_CRITICAL(日志管理器::获取实例().获取文件日志(), 格式,        \
                           ##__VA_ARGS__);                                     \
  } while (0)

#define 记录跟踪(格式, ...)                                                    \
  do {                                                                         \
    SPDLOG_LOGGER_TRACE(日志管理器::获取实例().获取控制台日志(), 格式,         \
                        ##__VA_ARGS__);                                        \
    SPDLOG_LOGGER_TRACE(日志管理器::获取实例().获取文件日志(), 格式,           \
                        ##__VA_ARGS__);                                        \
  } while (0)