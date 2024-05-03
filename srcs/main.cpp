#include <iostream>
#include "ServerConfig.hpp"
#include "ServerLocation.hpp"
#include "config_file.hpp"
#include "server_init.hpp"

void	print_server(const ServerConfig& server)
{
	std::cout << std::endl << "\t ==[ServerConfig details]==" << std::endl
		<< "port : " << server.getPort() << std::endl
		<< "server_names : ";
	std::cout << server.getServer_names()[0];
	for (size_t i = 1; i < server.getServer_names().size(); i++)
	{
		std::cout << ", " << server.getServer_names()[i];
	}
	std::cout << std::endl
		<< "default : " << server.isDefault() << std::endl
		<< "err_codes : ";
	std::map<int, std::string> err_codes = server.getErr_codes();
	for (std::map<int, std::string>::const_iterator it = err_codes.begin(); it != err_codes.end(); ++it)
		std::cout << it->first << " = " << it->second << "; ";
	std::cout << std::endl
		<< "body_size : " << server.getBody_size() << std::endl
		<< "locations : " << std::endl;
	for (size_t i = 0; i < server.getLocations().size(); i++)
	{
		ServerLocation	tmp = server.getLocations()[i];
		std::cout << "\t" << tmp.getLocation() << std::endl;
		std::cout << "\t- http_methods : GET(" << ((tmp.getHttp_methods() & GET) != 0)
			<< "), POST(" << ((tmp.getHttp_methods() & POST) != 0)
			<< "), DELETE(" << ((tmp.getHttp_methods() & DELETE) != 0) << ")" << std::endl;
		for (std::vector<t_rewrite>::const_iterator it = tmp.getRewrite().begin(); it != tmp.getRewrite().end(); it++)
			std::cout << "\t- rewrite : " << it->first << " " << it->second << std::endl;
		std::cout << "\t- root : " << tmp.getRoot() << std::endl;
		std::cout << "\t- autoindex : " << tmp.getAutoindex() << std::endl;
		std::cout << "\t- index : " << tmp.getIndex() << std::endl;
		std::cout << "\t- cgi : ";
		std::map<std::string, std::string> cgi = tmp.getCgi();
		for (std::map<std::string, std::string>::const_iterator it = cgi.begin(); it != cgi.end(); ++it)
			std::cout << it->first << " = " << it->second << "; ";
		std::cout << std::endl;
		std::cout << "\t- upload_store : " << tmp.getUpload_store() << std::endl;
	}
}

int	main(int argc, char *argv[])
{
	std::vector<ServerConfig> config;

	if (argc != 2)
	{
		std::cerr << "Usage : " << argv[0] << "<config_file>" << std::endl;
		return 1;
	}

	// for (std::vector<ServerConfig>::iterator it = config.begin(); it != config.end(); it++)
	// 	print_server(*it);
	// std::cout << std::endl;

	if (!load_config(argv[1], config))
		return 1;
	return server_init(config);
}
