#pragma once

#include <asm-generic/socket.h>
#include <cstdint>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <netinet/in.h>


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


struct conn_info {
    struct sockaddr_in addr;
    int fd         = -1;
    bool conn_succ = true;
};

struct conn_addr_info {
    std::string ip;
    uint16_t port;
};

int open_socket ();
void set_socket_options (const int sock_fd);
void bind_socket (const int sock_fd, struct sockaddr_in& sock_addr);
void listen_socket (const int sock_fd);
void close_socket (const int sock_fd);
void handle_conns (const int sock_fd);
conn_addr_info get_conn_info (const struct sockaddr_in& addr);
conn_info accept_conn (const int sock_fd);
