#pragma once

#include <cstring>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>


const int SOCK_FAM = AF_INET; // ipv4
const int SOCK_TYPE = SOCK_STREAM; // stream socket, others are datagram(UDP, ig), seq packet and raw packet
const int PROTOCOL = IPPROTO_TCP; // or 0, 1 for UDP, ...

int open_socket ();
void close_socket (const int sock_fd);
