#pragma once

#include <netinet/in.h>
#include <string>

struct conn_info {
    struct sockaddr_in addr;
    int fd         = -1;
    bool conn_succ = true;
};

struct conn_addr_info {
    std::string ip;
    uint16_t port;
};

struct error_status_info {
    bool errored = false;
    std::string msg;
};
