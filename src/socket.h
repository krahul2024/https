#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <sstream>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <netinet/in.h>
#include <unordered_map>
#include <map>

#include "../utils/methods.h"
#include "../utils/log.h"
#include "../utils/colors.h"

const int SOCK_FAM  = AF_INET; // ipv4
const int SOCK_TYPE = SOCK_STREAM; // stream socket, others are datagram(UDP, ig), seq packet and raw packet
const int PROTOCOL  = IPPROTO_TCP; // or 0, 1 for UDP, ...

// Set Socket Option constraints
const int SOCK_OPT_LVL    = SOL_SOCKET; // generic socket options
const int SOCK_OPT_NAME   = SO_REUSEADDR;
const int SOCK_OPT_ACTIVE = 1; //

// Bind Constraints
const int SOCK_ADD_IN_FAMILY     = AF_INET; // ipv4, ig
const uint16_t SOCK_PORT         = 5000;
const in_addr_t SOCK_LISTEN_ADDR = INADDR_ANY; // 0.0.0.0: all interafaces/clients, might set to localhost 127.0.0.1
const in_addr_t HOST_ADDR        = htonl (0x7f000001); // 127.0.0.1: localhost, ig

// Listen Constraints
const int32_t BACKLOG_Q = 5;

// Request Constraints
const size_t READ_CHUNK_SIZE = 4096;
constexpr const char* CRLF = "\r\n";
constexpr const char* DOUBLE_CRLF = "\r\n\r\n";
constexpr const char* SPACE = " ";


struct conn_info {
    struct sockaddr_in addr;
    int fd         = -1;
    bool conn_succ = true;
};

struct conn_addr_info {
    std::string ip;
    uint16_t port;
};

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

struct error_status_info {
    bool errored = false;
    std::string msg;
};

struct HttpRequest {
    METHOD::TYPE method;
    HTTP_VERSION::VERSION http_version;
    std::string path;
    std::string query_string;
    std::map<std::string, std::string> headers;
    std::string body;

    // -> status, error_msg
    static error_status_info set_method_path_version_from_req (std::string& req_str, HttpRequest& h) {
        std::string line = util_methods::consume_str_till_token (req_str, CRLF);
        if (line.size() == 0)
            return {true, "invalid request body"};

        std::string method_str = util_methods::consume_str_till_token (line, SPACE);
        h.method = METHOD::from_string (method_str);
        if (h.method == METHOD::TYPE::NONE)
            return { true, "invalid method, bad request" };

        h.path = util_methods::consume_str_till_token (line, SPACE);
        h.http_version = HTTP_VERSION::from_string(line);
        if (h.http_version == HTTP_VERSION::VERSION::NONE)
            return { true, "invalid http version, bad request" };

        return {false, ""};
    }

    // -> status, error_msg
    static error_status_info set_headers_from_req (std::string& req_str, HttpRequest& h) {
        std::string headers_str = util_methods::consume_str_till_token (req_str, DOUBLE_CRLF);

        if (headers_str.empty()) {
            h.body = req_str;
            return { false, "" };
        }

        while (!headers_str.empty()) {
            std::string header_line = util_methods::consume_str_till_token (headers_str, CRLF);

            if (header_line.empty())
                break;

            std::string key = util_methods::consume_str_till_token (header_line, ":");
            if (key.empty())
                return { true, "malformed header: missing key/value" };

            std::string value = util_methods::trim_whitespace (header_line);
            h.headers.insert ({ key, value });
        }

        return {false, ""};
    }

    static void log_req_line(const HttpRequest& h) {
        utils::log (
            "[", util_methods::get_timestamp (), "] ",
            METHOD::to_string (h.method), " ",
            h.path,
            (h.query_string.empty () ? "" : "?" + h.query_string), " ",
            HTTP_VERSION::to_string (h.http_version)
        );
    }

    static void log_headers (const HttpRequest& h) {
        utils::log("[", util_methods::get_timestamp (), "] Headers:");

        for (const auto& [key, value] : h.headers)
            utils::log("  ", key, ": ", value);
    }

    static void log_request (const HttpRequest& h, const std::string& client_ip) {
        std::ostringstream oss;

        oss << "[" << util_methods::get_timestamp_with_tz() << "] ";

        oss << util_colors::yellow;
        oss << client_ip << " ";
        oss << "\"" << METHOD::to_string(h.method) << " " << h.path;

        if (!h.query_string.empty())
            oss << "?" << h.query_string;

        oss << " " << HTTP_VERSION::to_string(h.http_version) << "\" ";
        oss << util_colors::blue;

        // headers
        oss << "headers={";
        bool first = true;

        for (const auto& [key, value] : h.headers) {
            if (!first) oss << ", ";
            oss << key << ":\"" << value << "\"";
            first = false;
        }

        oss << "}";
        oss << util_colors::reset;

        utils::log(oss.str());
    }
};

int open_socket ();
void set_socket_options (const int sock_fd);
void bind_socket (const int sock_fd, struct sockaddr_in& sock_addr);
void listen_socket (const int sock_fd);
void close_socket (const int sock_fd);
void handle_conns (const int sock_fd);

conn_addr_info get_conn_info (const struct sockaddr_in& addr);
conn_info accept_conn (const int sock_fd);
error_status_info read_req (const conn_info& client_conn_info, const conn_addr_info& ca_info);
std::pair<bool, std::string> read_req_data (const int __fd);
