#include "./main.h"
#include "../utils/log.h"
#include "../utils/error.h"

int main() {
    auto sock_fd = open_socket ();
    utils::log ("Socket fd = ", sock_fd);

    close_socket (sock_fd);
}

int open_socket () {
    int sock_fd = socket (SOCK_FAM, SOCK_TYPE, PROTOCOL);

    if (sock_fd < 0)
        utils::throw_error ("Socket creation error: ", strerror (errno));

    return sock_fd;
}

void close_socket (const int sock_fd) {
    int close_res = close (sock_fd);
    if (close_res < 0)
        utils::throw_error ("Error closing the socket: ", strerror (errno));

    utils::log ("Closed the socket successfully, Code = ", close_res);
}
