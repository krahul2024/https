#pragma once

#include <iostream>
#include <stdexcept>
#include <mutex>
#include <sstream>


namespace utils {

inline std::mutex utils_log_mut;

template<typename... Args>
inline void log (Args&&... args) {
    std::ostringstream oss;
    (oss << ... << args);

    {
        std::lock_guard<std::mutex> lock(utils_log_mut);
        std::cout << oss.str() << std::endl;
    }
}

} // namespace utils
