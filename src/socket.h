#pragma once

#include "./constants.h"
#include "./defs.h"
#include "../utils/methods.h"
#include "../utils/log.h"
#include "../utils/colors.h"


int open_socket ();
void set_socket_options (const int sock_fd);
void bind_socket (const int sock_fd, struct sockaddr_in& sock_addr);
void listen_socket (const int sock_fd);
void close_socket (const int sock_fd);
void handle_conns (const int sock_fd);

conn_addr_info get_conn_info (const struct sockaddr_in& addr);
conn_info accept_conn (const int sock_fd);
error_status_info read_req (const conn_info& client_conn_info, const conn_addr_info& ca_info);
std::pair<bool, std::string> read_req_data (const int __fd);
