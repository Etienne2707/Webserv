#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

# include <string>
# include <vector>
# include <map>
# include <sstream>
# include <exception>
# include <iostream>
# include "ServerLocation.hpp"


class	ServerConfig
{
	private:
		int				            _port;
		std::vector<std::string>    _server_names;
		bool			            _default;
		std::map<int, std::string>	_err_codes;
		int				            _body_size;
		std::vector<ServerLocation>	_locations;

	public:
		ServerConfig();
		~ServerConfig();
		ServerConfig(const ServerConfig& old);
		ServerConfig&	operator=(const ServerConfig& old);

		const int&										getPort() const;
		const std::vector<std::string>&			        getServer_names() const;
		bool									        isDefault() const;
		const std::map<int, std::string>&				getErr_codes() const;
		const int&										getBody_size() const;
		const std::vector<ServerLocation>&		        getLocations() const;

		void	setPort(std::istringstream& port);
		void	addServer_names(std::istringstream& server_names);
		void	addErr_code(std::istringstream& err_code);
		void	setBody_size(std::istringstream& body_size);
		void	addLocation(ServerLocation& locations);


   		void  fillDefaults(const std::vector<ServerConfig>& config);

};

bool	cmp_vec(const std::pair<int, ServerLocation*>& a, const std::pair<int, ServerLocation*>& b);

#endif
