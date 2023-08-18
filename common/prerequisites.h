#pragma once
#include <array>
#include <functional>
#include <glm/glm.hpp>
#include <iostream>
#include <string_view>
#include <sstream>

#include "config.h"
#include "device.h"
#include "signal.h"
#include "singleton.h"

template <class... Args>
constexpr void myprint(std::ostream &os, Args &&...args) noexcept {
    ((os << std::forward<Args>(args) << " "), ...);
}

#ifdef NDEBUG
#define LOG(...)
#define LOG_VAR(str)
#else
#define LOG(...)                                          \
    do {                                                  \
        std::cout << __FILE__ << " " << __LINE__ << ": "; \
        myprint(std::cout, __VA_ARGS__);                  \
        std::cout << std::endl;                           \
    } while (false)
#define LOG_VAR(...)                                                             \
    do {                                                                         \
        std::cout << __FILE__ << " " << __LINE__ << " " << #__VA_ARGS__ << ": "; \
        myprint(std::cout, __VA_ARGS__);                                         \
        std::cout << std::endl;                                                  \
    } while (false)
// std::cout << __FILE__ << " " << __LINE__ << ":  " << #str << ": " << str <<
// std::endl;
#endif

#define THROW(...)                            \
    do {                                      \
        std::stringstream ss;                 \
        myprint(ss, __FILE__, __LINE__, ":"); \
        myprint(ss, __VA_ARGS__);             \
        throw std::runtime_error(ss.str());   \
    } while (false)
