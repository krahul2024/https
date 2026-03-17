#include "./main.h"
#include "./socket.h"
#include "../utils/log.h"


int main() {
    int sock_fd = open_socket ();

    struct sockaddr_in sock_addr;
    set_socket_options(sock_fd);
    bind_socket (sock_fd, sock_addr);

    listen_socket(sock_fd);
    handle_conns (sock_fd);

    close_socket (sock_fd);
}

