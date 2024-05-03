#ifndef PROCESS_REQUEST_HPP
# define PROCESS_REQUEST_HPP

# include <string>
# include <vector>
# include <sys/socket.h>
# include <sys/epoll.h>
# include <iostream>
# include <sys/types.h>
# include <algorithm>
# include <cstring>
# include <map>
# include "AHTTP.hpp"
# include "Response.hpp"
# include "Request.hpp"

# define IN_FORK 0
# define IN_MAIN 1

typedef struct	s_client
{
	int				port;
	std::string		ip;
	Request			request;
	Response		response;
	std::string		raw_response;
}	t_client;

typedef struct	s_cgi
{
	int			client_fd;
	std::string	raw_response;
}	t_cgi;

typedef struct	s_ongoing
{
	std::map<int, t_client>	requests;
	std::map<int, t_cgi>	cgis;
}	t_ongoing;

void		sendBasicErrorResponse(int error_code, t_client& client_data);
int     	send_response_to_client(t_client& client_data, std::string &body, int error);
void    	Content_Type(Response& get_response,Request & request);
int 		process_request(const std::vector<ServerConfig> &config_list, const std::string& raw_input, t_ongoing& ongoing, int packet_fd, int& epoll_fd, std::map<int, int>& server_sockets);
bool		process_cgi_request(t_ongoing& ongoing, int packet_fd);
void		clean_ongoing(std::map<int, int>& server_sockets, t_ongoing& ongoing, int& epoll_fd, int provenance);
bool		isLineEmpty(const std::string& line);

// cgi
std::string	uint_to_str(unsigned int n);
std::string	int_to_str(int n);
std::string	method_to_str(int method);
std::string	find_server_name(const Request& request, const ServerConfig& config);
void		fill_env(char*** env, const std::string& var, const std::string& value);
void		clear_env(char** env);
pid_t		prepare_cgi(int* pipe_in, int* pipe_out);
void		process_cgi_fork(int* pipe_in, int* pipe_out, t_client& client, const std::string& script);
void		process_cgi(t_client& client, const std::string& script, int packet_fd, const int& epoll_fd, std::map<int, t_cgi>& ongoing_cgi);
bool		handle_cgi(t_client& client, const std::vector<ServerLocation>& locations, int packet_fd, int& epoll_fd, t_ongoing& ongoing, std::map<int, int>& server_socket);
char**		create_env(const t_client& client, const std::string& script);

// get
void		get_inside_file(std::string &result, std::ifstream &file);
int 		location_auto_index_redirectory(ServerLocation location,std::string path,  std::string &response);
int			location_index_redirectory(ServerLocation location,std::string path,  std::string &response);
void    	get_fields_response(std::map<std::string, std::string> it, std::string& response, Response & get_response, Request & request);
int         handle_GET(t_client& client_data, ServerLocation& location, std::string& body);

// post
int    		handle_POST(t_client& client_data, const ServerLocation& matched_location);

// delete
int         handle_DELETE(t_client& client_data, std::string& body);


#endif
