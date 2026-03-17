#pragma once

#include <cctype>
#include <iomanip>
#include <string>
#include <chrono>
#include <sstream>

namespace util_methods {

inline bool str_compare (const std::string& s1, const std::string& s2, bool case_sensitive = false) {
    if (s1.size() != s2.size())
        return false;

    if (case_sensitive)
        return s1 == s2;

    return std::equal (s1.begin(), s1.end(), s2.begin(), s2.end(), [](char c1, char c2){
        return std::tolower (static_cast<unsigned char> (c1)) == std::tolower (static_cast <unsigned char> (c2));
    }); // casting to prevent negative chars undef behav...

};

inline std::string to_lower_str(const std::string& s) {
    std::string result = s;

    for (char& c : result)
        c = std::tolower (static_cast<unsigned char>(c));

    return result;
};

inline std::string to_upper_str(const std::string& s) {
    std::string result = s;

    for (char& c : result)
        c = std::toupper (static_cast<unsigned char>(c));

    return result;
}

inline std::string consume_str_till_token (std::string& s, const std::string& tok) {
    size_t pos = s.find(tok);

    // substr `tok` doesn't exist
    if (pos == std::string::npos)
        return "";

    std::string res = s.substr (0, pos);
    s.erase (0, pos + tok.length());
    return res;
};

inline std::string trim_leading_whitespace (const std::string& s) {
    if (s.empty()) return s;

    size_t start = s.find_first_not_of(" \t");
    if (start == std::string::npos)
        return "";

    return s.substr(start);
}

inline std::string trim_trailing_whitespace (const std::string& s) {
    if (s.empty()) return s;

    size_t end = s.find_last_not_of(" \t");
    if (end == std::string::npos)
        return "";

    return s.substr(0, end + 1);
}

inline std::string trim_whitespace (const std::string& s) {
    return trim_trailing_whitespace (trim_leading_whitespace (s));
}

inline std::string get_timestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;

    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    oss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return oss.str();
}

inline std::string get_timestamp_with_tz() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;

    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    oss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    oss << std::put_time(std::localtime(&time), " %Z");  // e.g., "IST", "UTC"

    return oss.str();
}

}
