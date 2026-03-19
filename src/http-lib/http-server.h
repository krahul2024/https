#pragma once

#include <cstdint>
#include <netinet/in.h>
class http_server {

public:
    http_server () {};

    ~http_server () {}; // not quite useful though

private:
    int sock_fd;
    uint16_t port;
    int backlog_queue_sz;
    sockaddr_in server_addr;

};
