#pragma once

#include "./error.h"
#include <optional>

template<typename T>
struct result {
    std::optional<T> value;  // actual value if success
    error error;              // holds error info if failure

    // constructor success
    result(T val) 
        : value(std::move(val))
        , error(error::success()) {} // set error as "none"

    // constructor failure
    result(struct error e)
        : value(std::nullopt) // no value
        , error(std::move(e)) {} // store the error

    bool ok() const { return error.ok(); }
    explicit operator bool() const { return ok(); }

    T& get() {
        if (!ok())
            utils::throw_error ("trying to get value from error: ", error.msg);

        return *value;
    }

    T& get() const {
        if (!ok())
            utils::throw_error ("trying to get value from error: ", error.msg);

        return *value;
    }

    T get_or (T default_val) const {
        return ok() ? *value: std::move (default_val);
    }
};

/*

result<std::string> read_request_data(int fd) {
    char buffer[4096];
    ssize_t n = read(fd, buffer, sizeof(buffer));

    if (n < 0)
        return error::from_errno(error_codes::read_error);

    return std::string(buffer, n);
}

result<HttpRequest> parse_request(const std::string& raw) {
    if (raw.empty())
        return error::make(error_codes::parse_error, "Empty");

    HttpRequest req;
    // ... parse ...
    return req;
}

auto x = read_request_data(fd);
if (!x.ok()) {
    utils::log(x.error.msg);
    return x.error;
}

std::string raw = *x.value;

// propagation of errors
result<HttpRequest> read_and_parse(int fd) {
    auto x = read_request_data(fd);
    if (!x.ok()) return x.error;

    auto y = parse_request(*x.value);
    if (!y.ok()) return y.error;

    return *y.value;
}

*/
