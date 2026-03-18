#pragma once

#include <map>
#include <unordered_map>

#include "./constants.h"
#include "./defs.h"
#include "../utils/methods.h"
#include "../utils/log.h"
#include "../utils/error.h"
#include "../utils/colors.h"
#include "./headers.h"

struct METHOD {
    enum class TYPE { GET, POST, PUT, DELETE, NONE };

    static std::string to_string (TYPE t) {
        std::string type;

        switch (t) {
            case TYPE::GET: type = "GET"; break;
            case TYPE::POST: type = "POST"; break;
            case TYPE::PUT: type = "PUT"; break;
            case TYPE::DELETE: type = "DELETE"; break;
            default: type = "NONE";
        }

        return type;
    }

    static TYPE from_string(const std::string& s) {
        static const std::unordered_map<std::string, TYPE> map = {
            {"GET", TYPE::GET},
            {"POST", TYPE::POST},
            {"PUT", TYPE::PUT},
            {"DELETE", TYPE::DELETE},
        };

        auto it = map.find(s);
        return (it != map.end()) ? it->second : TYPE::NONE;
    }
};

struct HTTP_VERSION {
    enum class VERSION { HTTP1_1, NONE };

    static std::string to_string (VERSION v) {
        return (v == VERSION::HTTP1_1 ? "HTTP/1.1": "NONE");
    }

    static VERSION from_string (const std::string& s) {
        return (s == "HTTP/1.1" ? VERSION::HTTP1_1 : VERSION::NONE);
    }
};


struct HttpRequest {
    METHOD::TYPE method;
    HTTP_VERSION::VERSION http_version;
    std::string path;
    std::string query_string;
    std::map<std::string, std::string> headers;
    std::map<std::string, std::string> __headers;
    std::string body;

    // parsing
    static error_status_info parse_method_path_version_from_req (std::string& req_str, HttpRequest& h);
    static error_status_info parse_headers_from_req (std::string& req_str, HttpRequest& h);
    static error_status_info parse_body_from_req (HttpRequest& h);

    // helpers
    static const std::string get_req_header (HttpRequest& h, const std::string& header_str);

    // logging
    static void log_req_line(const HttpRequest& h);
    static void log_headers (const HttpRequest& h);
    static void log_request (const HttpRequest& h, const std::string& client_ip);
};

