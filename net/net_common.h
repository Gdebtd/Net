#ifndef NET_COMMON_H
#define NET_COMMON_H

#include <memory>
#include <thread>
#include <mutex>
#include <deque>
//#include <optional>
#include <vector>
#include <iomanip>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <cstdint>

#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif

#define ASIO_STANDALONE
#include <asio/asio.hpp>

#endif /* NET_COMMON_H */