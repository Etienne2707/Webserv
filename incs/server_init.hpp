#ifndef SERVER_INIT_HPP
# define SERVER_INIT_HPP

#include <stdlib.h>
#include <arpa/inet.h>
#include <vector>
#include <set>
#include <map>
#include <iostream>
#include <string>
#include <cstring>
#include <cerrno>
#include <sstream>
#include <fcntl.h>
#include <sys/epoll.h>
#include <csignal>
#include "process_request.hpp"

#define MAX_EVENTS 10
#define BUFFER_SIZE 8192

int 	server_init(const std::vector<ServerConfig> &config);
void	handle_client(const std::vector<ServerConfig> &config, int& epoll_fd, struct epoll_event event, t_ongoing& ongoing, std::map<int, int>& server_sockets);
void	handle_cgi_client(int& epoll_fd, struct epoll_event event, t_ongoing& ongoing);
void	server_main_loop(int& epoll_fd, const std::vector<ServerConfig>& config, std::map<int, int>& server_sockets);
void	register_client(const int& epoll_fd, const int& server_socket, std::map<int, t_client>& ongoing_requests, int port);
void	add_client_to_epoll(const int& epoll_fd, const int& client_sock);
int		match_socket(const std::map<int, int>& server_sockets, const int& fd);
void	bind_server_socket(std::map<int, int>& server_sockets, const int& port, const int& epoll_fd);
void	handle_signal(int sig);

#endif
