#include "./socket.h"
#include "../utils/log.h"
#include "../utils/error.h"
#include <arpa/inet.h>
#include <cerrno>
#include <sys/socket.h>
#include <netinet/in.h>
#include <system_error>


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

    utils::log ("Closed the socket successfully, Code = ", res);
}

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
    utils::log ("Listening on ", client_ca_info.ip, ":", client_ca_info.port);
}

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
        utils::log ("Connection Accepted: IP = ", client_ca_info.ip, ":", client_ca_info.port);

        close_socket (client_conn_info.fd);
    }
}
