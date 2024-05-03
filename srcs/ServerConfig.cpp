#include "ServerConfig.hpp"

ServerConfig::ServerConfig()
{
	_port = -1;
	_default = false;
	_body_size = -1;
}

ServerConfig::~ServerConfig() {}

ServerConfig::ServerConfig(const ServerConfig& old)
{
	*this = old;
}

ServerConfig&	ServerConfig::operator=(const ServerConfig& old)
{
	if (this == &old)
		return *this;

	_port = old._port;
	_server_names = old._server_names;
	_default = old._default;
	_err_codes = old._err_codes;
	_body_size = old._body_size;
	_locations = old._locations;

	return *this;
}

const int&			ServerConfig::getPort() const
{
	return	_port;
}

const std::vector<std::string>&	ServerConfig::getServer_names() const
{
	return _server_names;
}

bool			ServerConfig::isDefault() const
{
	return	_default;
}

const std::map<int, std::string>&	ServerConfig::getErr_codes() const
{
	return	_err_codes;
}

const int&				ServerConfig::getBody_size() const
{
	return	_body_size;
}

const std::vector<ServerLocation>&	ServerConfig::getLocations() const
{
	return	_locations;
}

void	ServerConfig::setPort(std::istringstream& port)
{
	if (_port != -1)
		throw std::runtime_error("Duplicate port entry found.");
	if (!(port >> _port) || _port < 0 || _port > 65535)
		throw std::runtime_error("Invalid port value.");
	if (!port.eof())
		throw std::runtime_error("Too many arguments for port entry.");
}

void	ServerConfig::addServer_names(std::istringstream& server_names)
{
	std::string server_name;

	while (server_names >> server_name)
		_server_names.push_back(server_name);
}

void	ServerConfig::addErr_code(std::istringstream& err_code)
{
	int	code;
	std::string file_path;

	if (!(err_code >> code) || code < 0)
		throw std::runtime_error("Invalid error_code value.");
	if (_err_codes.count(code))
		throw std::runtime_error("Duplicate error_code entry found.");
	if (!(err_code >> file_path))
		throw std::runtime_error("Missing file path entry for error_code.");
	_err_codes[code] = file_path;
	if (!err_code.eof())
		throw std::runtime_error("Too many arguments for error_code entry.");
}

void	ServerConfig::setBody_size(std::istringstream& body_size)
{
	if (_body_size != -1)
		throw std::runtime_error("Duplicate body_size entry found.");
	if (!(body_size >> _body_size) || _body_size < 0)
		throw std::runtime_error("Invalid body_size value.");
	if (!body_size.eof())
		throw std::runtime_error("Too many arguments for body_size entry.");
}

void	ServerConfig::addLocation(ServerLocation& location)
{
	for (std::vector<ServerLocation>::iterator it = _locations.begin(); it != _locations.end(); it++)
	{
		if (it->getLocation() == location.getLocation())
			throw std::runtime_error("Duplicate location block found.");
	}
	_locations.push_back(location);
}

bool	cmp_vec(const std::pair<int, ServerLocation*>& a, const std::pair<int, ServerLocation*>& b)
{
	if (a.first <= b.first)
		return true;
	return false;
}

void  ServerConfig::fillDefaults(const std::vector<ServerConfig>& config)
{
	bool  isDefault = true;
	bool  hasRoot = false;
	std::vector< std::pair<int, ServerLocation*> >	v_locations;

	// Check if port is missing
	if (_port == -1)
		throw std::runtime_error("Port entry missing in server block.");

	// Check if "/" location is missing and create a vector to fill defaults later
	for (std::vector<ServerLocation>::size_type it = 0; it < _locations.size(); it++)
	{
		int	i = 0;
		std::string str = _locations[it].getLocation();
		for (std::string::size_type j = 0; j < str.length(); j++)
		{
			if (str[j] == '/')
				i++;
		}
		v_locations.push_back(std::make_pair(i, &_locations[it]));
		if (_locations[it].getLocation() == "/")
			hasRoot = true;
	}
	if (!hasRoot)
		throw std::runtime_error("Location '/' missing in server block.");

	// Set server as default if needed
	for (std::vector<ServerConfig>::const_iterator it = config.begin(); it != config.end() && isDefault; it++)
	{
		if (it->_port == _port)
			isDefault = false;
	}
	_default = isDefault;

	// Fill default values in order
	std::sort(v_locations.begin(), v_locations.end(), &cmp_vec);
	for (std::vector< std::pair<int, ServerLocation*> >::iterator it = v_locations.begin(); it != v_locations.end(); it++)
		it->second->fillDefaults(_locations);
}
