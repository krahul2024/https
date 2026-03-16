#pragma once

#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <cstring>
#include <mutex>
#include <sstream>


namespace utils {

inline std::mutex utils_err_mut;

template<typename... Args>
[[noreturn]] inline void throw_error (Args&&... args) {
    std::ostringstream oss;
    (oss << ... << args);

    {
        std::lock_guard<std::mutex> lock (utils_err_mut);
        std::cerr << oss.str() << std::endl;
    }

    throw std::runtime_error (oss.str());
}

inline const char* errno_str (const int err_num) {
    return strerror (err_num);
}

}
