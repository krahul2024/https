#include <arpa/inet.h>
#include <cerrno>
#include <utility>

#include "./socket.h"
#include "http.h"
#include "../utils/log.h"
#include "../utils/error.h"

int open_socket () {
    int sock_fd = socket (SOCK_FAM, SOCK_TYPE, PROTOCOL);

    if (sock_fd < 0)
        utils::throw_error ("Socket creation error: ", strerror (errno));

    return sock_fd;
}

void set_socket_options (const int sock_fd) {
    int res = setsockopt (sock_fd, SOCK_OPT_LVL, SOCK_OPT_NAME, &SOCK_OPT_ACTIVE, sizeof (SOCK_OPT_ACTIVE));
    if (res < 0)
        utils::throw_error ("Error setting socket options: ", strerror (errno));
}

void bind_socket (const int sock_fd, struct sockaddr_in& sock_addr) {
    memset (&sock_addr, 0, sizeof (sock_addr));
    sock_addr.sin_family      = SOCK_ADD_IN_FAMILY;
    sock_addr.sin_port        = htons (SOCK_PORT);
    sock_addr.sin_addr.s_addr = HOST_ADDR;

    int res = bind (sock_fd, (struct sockaddr*)&sock_addr, sizeof (sock_addr));
    if (res < 0) {
        memset (&sock_addr, 0, sizeof (sock_addr));
        utils::throw_error ("Error binding socket: ", strerror (errno));
    }
}

void close_socket (const int sock_fd) {
    int res = close (sock_fd);
    if (res < 0)
        utils::throw_error ("Error closing the socket: ", strerror (errno));
}

// -> connection address info(ip, port)
conn_addr_info get_conn_info (const struct sockaddr_in& addr) {
    conn_addr_info ca_info;
    char ip_str[INET_ADDRSTRLEN];

    inet_ntop (SOCK_ADD_IN_FAMILY, &addr.sin_addr, ip_str, sizeof (ip_str));

    ca_info.ip   = ip_str;
    ca_info.port = ntohs (addr.sin_port);

    return ca_info;
}

void listen_socket (const int sock_fd) {
    int res = listen (sock_fd, BACKLOG_Q);
    if (res < 0)
        utils::throw_error ("Error listening: ", strerror (errno));

    struct sockaddr_in addr{};
    socklen_t addr_len = sizeof (addr);

    // just for fun and re-check, else could re-use the already formed struct
    if (getsockname (sock_fd, (struct sockaddr*)& addr, &addr_len) < 0)
        utils::throw_error ("get socket name error: ", strerror (errno));

    const conn_addr_info client_ca_info = get_conn_info(addr);
    utils::log (util_colors::green, "---- Listening on ", client_ca_info.ip, ":", client_ca_info.port, " ----", util_colors::reset);
}

// -> connection info
conn_info accept_conn (const int sock_fd) {
    struct conn_info client_conn;
    memset (&client_conn.addr, 0, sizeof (struct sockaddr_in));

    socklen_t client_addr_len = sizeof (client_conn.addr);
    client_conn.fd            = accept (sock_fd, (struct sockaddr*)&client_conn.addr, &client_addr_len);

    if (client_conn.fd < 0) {
        client_conn.conn_succ = false;
        utils::log ("Error accepting connection: ", strerror (errno));
    }

    return client_conn;
}

void handle_conns (const int sock_fd) {
    while (true) {
        const conn_info client_conn_info = accept_conn (sock_fd);
        if (!client_conn_info.conn_succ)
            continue;

        const conn_addr_info client_ca_info = get_conn_info (client_conn_info.addr);
        auto err_status_info                = read_req (client_conn_info, client_ca_info);
        if (err_status_info.errored) {
            utils::log (err_status_info.msg);
        }

        close_socket (client_conn_info.fd);
    }
}

error_status_info read_req (const conn_info& client_conn_info, const conn_addr_info& ca_info) {
    auto [read_error, request_str] = read_req_data (client_conn_info.fd);
    if (read_error)
        return {true, "Error reading the request"};

    HttpRequest h_req;
    auto err_info = HttpRequest::set_method_path_version_from_req (request_str, h_req);
    if (err_info.errored)
        return err_info;

    err_info = HttpRequest::set_headers_from_req (request_str, h_req);
    if (err_info.errored)
        return err_info;

    HttpRequest::log_request (h_req, ca_info.ip);
    return { false, "" };
}

// -> (read_success, request_string)
std::pair<bool, std::string> read_req_data (const int __fd) {
    std::string request;
    bool read_error = false;
    char buffer [READ_CHUNK_SIZE];

    while (true) {
        ssize_t n_bytes = read (__fd, buffer, READ_CHUNK_SIZE);
        if (n_bytes < 0) {
            utils::log ("Request Read error: ", strerror (errno));
            read_error = true;
            break;
        } else if (n_bytes == 0) {
            utils::log ("Connection closed by client.");
            read_error = true;
            break;
        }

        request.append (buffer, n_bytes);

        if (request.find (DOUBLE_CRLF) != std::string::npos)
            break;
    }

    return std::make_pair (read_error, request);
}
