#pragma once
#include <string>

inline void str_tolower(std::string &s) {
    for (auto &c : s) c = std::tolower(c);
}