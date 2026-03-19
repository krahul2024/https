#pragma once

#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <cstring>
#include <mutex>
#include <sstream>

struct http_status {
    // 2xx
    static constexpr int ok = 200;
    static constexpr int created = 201;
    static constexpr int no_content = 204;

    // 3xx
    static constexpr int moved_permanently = 301;
    static constexpr int found = 302;
    static constexpr int not_modified = 304;

    // 4xx
    static constexpr int bad_request = 400;
    static constexpr int unauthorized = 401;
    static constexpr int forbidden = 403;
    static constexpr int not_found = 404;
    static constexpr int method_not_allowed = 405;
    static constexpr int request_timeout = 408;
    static constexpr int conflict = 409;
    static constexpr int payload_too_large = 413;
    static constexpr int uri_too_long = 414;
    static constexpr int unsupported_media_type = 415;
    static constexpr int too_many_requests = 429;

    // 5xx
    static constexpr int internal_server_error = 500;
    static constexpr int not_implemented = 501;
    static constexpr int bad_gateway = 502;
    static constexpr int service_unavailable = 503;
    static constexpr int gateway_timeout = 504;

    static const char* reason_phrase(int code) {
        switch (code) {
            case 200: return "OK";
            case 201: return "Created";
            case 204: return "No Content";
            case 301: return "Moved Permanently";
            case 302: return "Found";
            case 304: return "Not Modified";
            case 400: return "Bad Request";
            case 401: return "Unauthorized";
            case 403: return "Forbidden";
            case 404: return "Not Found";
            case 405: return "Method Not Allowed";
            case 408: return "Request Timeout";
            case 409: return "Conflict";
            case 413: return "Payload Too Large";
            case 414: return "URI Too Long";
            case 415: return "Unsupported Media Type";
            case 429: return "Too Many Requests";
            case 500: return "Internal Server Error";
            case 501: return "Not Implemented";
            case 502: return "Bad Gateway";
            case 503: return "Service Unavailable";
            case 504: return "Gateway Timeout";
            default: return "Unknown";
        }
    }
};

enum class error_code {
    none = 0,
    unknown,

    read_error,
    write_error,
    connection_closed,
    timeout_error,

    parse_error,
    invalid_request,
    invalid_method,
    invalid_path,
    invalid_version,
    malformed_headers,
    header_too_large,
    body_too_large,
    missing_host_header,

    route_not_found,
    method_not_allowed,

    internal_error,
    not_implemented,
};

struct error {
    error_code code = error_code::none;
    std::string msg;
    int status_code = 0;

    bool ok() const {
        return code == error_code::none;
    }

    // explicit bool conversion (true = has error)
    explicit operator bool() const {
        return !ok();
    }

    static error success() {
        return {error_code::none, "", 0};
    }

    static error make (error_code code, const std::string& custom_msg = "", int custom_status = 0) {
        std::string msg = custom_msg.empty() ? default_message(code) : custom_msg;
        int status = custom_status != 0 ? custom_status : default_http_status(code);
        return {code, msg, status};
    }

    static error from_errno (error_code code) {
        return {code, std::strerror(errno), default_http_status(code)};
    }

    int http_status() const {
        return status_code != 0 ? status_code : default_http_status(code);
    }

private:
    static std::string default_message (error_code code) {
        switch (code) {
            case error_code::none:                return "Success";
            case error_code::unknown:             return "Unknown error";
            case error_code::read_error:          return "Failed to read request";
            case error_code::write_error:         return "Failed to write response";
            case error_code::connection_closed:   return "Connection closed by peer";
            case error_code::timeout_error:       return "Connection timed out";
            case error_code::parse_error:         return "Failed to parse request";
            case error_code::invalid_request:     return "Invalid request format";
            case error_code::invalid_method:      return "Invalid HTTP method";
            case error_code::invalid_path:        return "Invalid request path";
            case error_code::invalid_version:     return "Invalid HTTP version";
            case error_code::malformed_headers:   return "Malformed headers";
            case error_code::header_too_large:    return "Header too large";
            case error_code::body_too_large:      return "Request body too large";
            case error_code::missing_host_header: return "Missing Host header";
            case error_code::route_not_found:     return "Route not found";
            case error_code::method_not_allowed:  return "Method not allowed";
            case error_code::internal_error:      return "Internal server error";
            case error_code::not_implemented:     return "Not implemented";
            default:                              return "Unknown error";
        }
    }

    static int default_http_status(error_code code) {
        switch (code) {
            case error_code::none:                return http_status::ok;
            case error_code::parse_error:         return http_status::bad_request;
            case error_code::invalid_request:     return http_status::bad_request;
            case error_code::invalid_method:      return http_status::bad_request;
            case error_code::invalid_path:        return http_status::bad_request;
            case error_code::invalid_version:     return http_status::bad_request;
            case error_code::malformed_headers:   return http_status::bad_request;
            case error_code::header_too_large:    return http_status::bad_request;
            case error_code::body_too_large:      return http_status::payload_too_large;
            case error_code::missing_host_header: return http_status::bad_request;
            case error_code::route_not_found:     return http_status::not_found;
            case error_code::method_not_allowed:  return http_status::method_not_allowed;
            case error_code::timeout_error:       return http_status::request_timeout;
            case error_code::read_error:          return http_status::internal_server_error;
            case error_code::write_error:         return http_status::internal_server_error;
            case error_code::connection_closed:   return http_status::bad_request;
            case error_code::internal_error:      return http_status::internal_server_error;
            case error_code::not_implemented:     return http_status::not_implemented;
            default:                              return http_status::internal_server_error;
        }
    }
};

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
