#pragma once

#include <asm-generic/socket.h>
#include <cstdint>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>
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

// Listen Constraints
const int32_t BACKLOG_Q = 5;

int open_socket ();
void set_socket_options (const int sock_fd);
void bind_socket (const int sock_fd, struct sockaddr_in& sock_addr);
void listen_socket (const int sock_fd);
void close_socket (const int sock_fd);
