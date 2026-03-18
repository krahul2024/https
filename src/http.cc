#include "./http.h"


error_status_info HttpRequest::parse_method_path_version_from_req (std::string& req_str, HttpRequest& h) {
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
error_status_info HttpRequest::parse_headers_from_req (std::string& req_str, HttpRequest& h) {
    std::string headers_str = util_methods::consume_str_till_token (req_str, DOUBLE_CRLF);
    h.body                  = req_str;

    if (headers_str.empty())
        return { false, "" };

    while (!headers_str.empty()) {
        std::string header_line = util_methods::consume_str_till_token (headers_str, CRLF);

        if (header_line.empty())
            break;

        std::string key = util_methods::consume_str_till_token (header_line, ":");
        if (key.empty())
            return { true, "malformed header: missing key/value" };

        std::string value = util_methods::trim_whitespace (header_line);
        h.headers.insert ({ util_methods::to_lower_str(key), util_methods::to_lower_str(value) });
    }

    return {false, ""};
}

error_status_info HttpRequest::parse_body_from_req (HttpRequest& h) {
    if (h.body.empty())
        return { false, "" };

    const std::string& content_type = HttpRequest::get_req_header (h, http_headers::content_type);
    if (content_type.empty())
        return { true, "error; missing header \"content-type\"" };

    return { false, "" };
}

const std::string HttpRequest::get_req_header (HttpRequest& h, const std::string& header_str) {
    auto exists = h.headers.find (header_str);
    return (exists != h.headers.end()? h.headers.at (header_str): "");
}

void HttpRequest::log_req_line (const HttpRequest& h) {
    utils::log (
        "[", util_methods::get_timestamp (), "] ",
        METHOD::to_string (h.method), " ",
        h.path,
        (h.query_string.empty () ? "" : "?" + h.query_string), " ",
        HTTP_VERSION::to_string (h.http_version)
    );
}

void HttpRequest::log_headers (const HttpRequest& h) {
    utils::log("[", util_methods::get_timestamp (), "] Headers:");

    for (const auto& [key, value] : h.headers)
    utils::log("  ", key, ": ", value);
}

void HttpRequest::log_request (const HttpRequest& h, const std::string& client_ip) {
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
