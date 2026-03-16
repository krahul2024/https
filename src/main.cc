#include "./main.h"
#include "./socket.h"
#include "../utils/log.h"


int main() {
    auto sock_fd = open_socket ();
    utils::log ("Socket Opened with ID = ", sock_fd);

    set_socket_options(sock_fd);

    struct sockaddr_in sock_addr;
    bind_socket (sock_fd, sock_addr);

    listen_socket(sock_fd);

    close_socket (sock_fd);
}
