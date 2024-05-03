#include "ServerLocation.hpp"

ServerLocation::ServerLocation()
{
	_location = "";
	_http_methods = -1;
	_root = "";
	_autoindex = -1;
	_index = "";
	_upload_store = "";
}

ServerLocation::~ServerLocation() {}

ServerLocation::ServerLocation(const ServerLocation& old)
{
	*this = old;
}

ServerLocation& ServerLocation::operator=(const ServerLocation& old)
{
	if (this == &old)
		return *this;

	_http_methods = old._http_methods;
	_rewrite = old._rewrite;
	_root = old._root;
	_autoindex = old._autoindex;
	_index = old._index;
	_cgi = old._cgi;
	_location = old._location;
	_upload_store = old._upload_store;
	return *this;
}

const int&				ServerLocation::getHttp_methods() const
{
	return _http_methods;
}

const std::vector<t_rewrite>&		ServerLocation::getRewrite() const
{
	return _rewrite;
}

const std::string&		ServerLocation::getRoot() const
{
	return _root;
}

bool			ServerLocation::getAutoindex() const
{
	return (_autoindex == 1);
}

const std::string&		ServerLocation::getIndex() const
{
	return _index;
}

const std::map<std::string, std::string>&	ServerLocation::getCgi() const
{
	return _cgi;
}

const std::string&		ServerLocation::getLocation() const
{
	return _location;
}

const std::string&	ServerLocation::getUpload_store() const
{
	return _upload_store;
}

void	ServerLocation::setHttp_methods(std::istringstream& http_methods)
{
	int	n = 0;
	int method;
	std::string str;

	if (_http_methods != -1)
		throw std::runtime_error("Duplicate http_methods entry found.");

	while (http_methods >> str)
	{
		if (str == "GET")
			method = GET;
		else if (str == "POST")
			method = POST;
		else if (str == "DELETE")
			method = DELETE;
		else
			throw std::runtime_error("Invalid parameter for http_methods.");
		if (n & method)
			throw std::runtime_error("Duplicate parameter found for http_methods.");
		n |= method;
	}
	_http_methods = n;
}

void	ServerLocation::addRewrite(std::istringstream& rewrite)
{
	t_rewrite	tmp;

	if (!(rewrite >> tmp.first))
		throw std::runtime_error("Invalid parameter for rewrite instruction.");
	if (!(rewrite >> tmp.second))
		throw std::runtime_error("Invalid parameter for rewrite instruction.");
	if (!rewrite.eof())
		throw std::runtime_error("Too many arguments for rewrite instruction.");
	for (std::vector<t_rewrite>::const_iterator it = _rewrite.begin(); it != _rewrite.end(); it++)
	{
		if (it->first == tmp.first)
			throw std::runtime_error("Duplicate rewrite entry found.");
	}
	_rewrite.push_back(tmp);
}

void	ServerLocation::setRoot(std::istringstream& root)
{
	if (_root != "")
		throw std::runtime_error("Duplicate root entry found.");
	if (!(root >> _root) || is_path_valid(_root) == false)
		throw std::runtime_error("Invalid parameter for root instruction.");
	if (!root.eof())
		throw std::runtime_error("Too many arguments for root instruction.");
}

void	ServerLocation::setAutoindex(std::istringstream& autoindex)
{
	std::string str;

	if (_autoindex != -1)
		throw std::runtime_error("Duplicate autoindex entry found.");
	if (!(autoindex >> str))
		throw std::runtime_error("Invalid parameter for autoindex instruction.");
	if (str == "true")
		_autoindex = 1;
	else if (str == "false")
		_autoindex = 0;
	else
		throw std::runtime_error("Invalid parameter for autoindex instruction.");
	if (str == "true")
		if (!autoindex.eof())
			throw std::runtime_error("Too many arguments for autoindex instruction.");
}

void	ServerLocation::setIndex(std::istringstream& index)
{
	if (_index != "")
		throw std::runtime_error("Duplicate index entry found.");
	if (!(index >> _index))
		throw std::runtime_error("Invalid parameter for index instruction.");
	if (!index.eof())
		throw std::runtime_error("Too many arguments for index instruction.");
}

void	ServerLocation::addCgi(std::istringstream& cgi)
{
	std::string	ext;
	std::string file_path;

	if (!(cgi >> ext) || ext == ".")
		throw std::runtime_error("Invalid cgi extension value.");
	if (ext[0] == '.')
		ext = ext.substr(1);
	if (_cgi.count(ext))
		throw std::runtime_error("Duplicate cgi entry found.");
	if (!(cgi >> file_path))
		throw std::runtime_error("Invalid file path entry for cgi.");
	_cgi[ext] = file_path;
	if (!cgi.eof())
		throw std::runtime_error("Too many arguments for cgi entry.");
}

void	ServerLocation::setLocation(std::istringstream& location)
{
	if (!(location >> _location) || is_path_valid(_location) == false)
		throw std::runtime_error("Invalid location parameter value.");
	if (!location.eof())
		throw std::runtime_error("Too many arguments for location block.");
}

void	ServerLocation::addUpload_store(std::istringstream& upload_store)
{
	if (_upload_store != "")
		throw std::runtime_error("Duplicate upload_store entry found.");
	if (!(upload_store >> _upload_store))
		throw std::runtime_error("Invalid upload_store parameter value.");
	if (!upload_store.eof())
		throw std::runtime_error("Too many arguments for upload_store entry.");
}

void  ServerLocation::fillDefaultRoot(const ServerLocation& location)
{
	_root = location.getRoot();
	_root.append(_location.substr(location.getLocation().length()));
}

void  ServerLocation::fillDefaultAutoindex(const ServerLocation& location)
{
	if (location.getAutoindex())
		_autoindex = 1;
	else
		_autoindex = 0;
}

void  ServerLocation::fillDefaultRewrite(const std::vector<ServerLocation>& locations)
{
	std::vector<t_rewrite>	rewrite;
	t_rewrite	tmp;
	std::string	diff;
	bool		matching;

	for (std::vector<ServerLocation>::const_iterator it = locations.begin(); it != locations.end(); it++)
	{
		rewrite = it->getRewrite();
		diff = _location.substr(it->getLocation().length());
		for (std::vector<t_rewrite>::const_iterator jt = rewrite.begin(); jt != rewrite.end(); jt++)
		{
			matching = true;
			for (std::string::size_type i = 0; matching && i < diff.length(); i++)
			{
				if (diff[i] != jt->first[i + 1])
					matching = false;
			}
			if (matching)
			{
				tmp.first = jt->first.substr(diff.length());
				tmp.second = jt->second;
				for (std::vector<t_rewrite>::const_iterator it = _rewrite.begin(); it != _rewrite.end(); it++)
				{
					if (it->first == tmp.first)
						throw std::runtime_error("Duplicate rewrite entry found between location blocks.");
				}
				_rewrite.push_back(tmp);
			}
		}
	}
}

void  ServerLocation::fillDefaults(const std::vector<ServerLocation>& locations)
{
	std::vector<ServerLocation>	configLocations;
	std::string				path(_location);

	path.erase(path.end() - 1);
	if (!path.empty())
		path = path.substr(0, path.find_last_of('/') + 1);
	while (!path.empty())
	{
		for (std::vector<ServerLocation>::const_iterator it = locations.begin(); it != locations.end(); it++)
		{
			if ((*it).getLocation() == path)
			{
				configLocations.push_back(*it);
				break;
			}
		}
		path.erase(path.end() - 1);
		if (!path.empty())
			path = path.substr(0, path.find_last_of('/') + 1);
	}

	// Fill root
	if (_location == "/" && _root == "")
		throw std::runtime_error("Root entry missing in location '/'.");
	else if (_root == "")
		fillDefaultRoot(configLocations[0]);

	// Fill http_methods
	if (_http_methods == -1 && _location == "/")
		_http_methods = POST | GET;
	else if (_http_methods == -1)
		_http_methods = configLocations[0].getHttp_methods();

	// Fill autoindex
	if (_autoindex == -1 && _location == "/")
		_autoindex = 0;
	else if (_autoindex == -1)
		fillDefaultAutoindex(configLocations[0]);

	// Fill index
	if (_index == "" && _location != "/")
		_index = configLocations[0].getIndex();

	// Fill upload_store
	if (_upload_store == "" && _location != "/")
		_upload_store = configLocations[0].getUpload_store();

	// Fill rewrite
	fillDefaultRewrite(configLocations);
}

bool		is_path_valid(std::string& path)
{
	if (path[0] != '/')
		return false;
	if (path.find("//") != std::string::npos || path.find("?") != std::string::npos)
		return false;
	if (*(path.end() - 1) != '/')
		path.push_back('/');
	return true;
}
