#include "Request.hpp"

Request::Request(void): _empty(true), _full(false), _headerfields(false), _body_size(0), _chunked_length(0), _error_code(0), _to_close(false)
{
    _content_length = 0;
}

Request::~Request(void) {}

Request::Request(const Request& src)
{
    *this = src;
}

const Request&  Request::operator=(const Request& src)
{
    if (this != &src)
    {
        _http_method = src._http_method;
        _path =  src._path;
        _empty = src._empty;
        _full = src._full;
        _fieldbuffer = src._fieldbuffer;
        _fields = src._fields;
		_body = src._body;
		_content_length = src._content_length;
		_chunked_length = src._chunked_length;
		_body_size = src._body_size;
		_headerfields = src._headerfields;
		_location = src._location;
		_query = src._query;
		_path_info = src._path_info;
		_path_translated = src._path_translated;
		_config = src._config;
		_error_code = src._error_code;
		_transfer_encoding = src._transfer_encoding;
		_to_close = src._to_close;
		_cookie = src._cookie;
    }
    return *this;
}

int		Request::check_syntax_header_value(std::string & value)
{
	std::size_t found;

	if ((!value.empty() && value[0] == 34 && value[value.size() - 1] == 34))
	{
		value = value.substr(1,value.size() - 2);
		for (size_t i = 0; i < inv_value.size(); i++)
		{
			found = value.find(inv_value[i]);
			if (found != std::string::npos)
			{
	//			std::cout << red << "Found has find in value : " << inv_value[i] << std::endl;
				return (-1);
			}
		}
	//	std::cout << "Nothing invalid for value" << std::endl;
		return (1);
	}
	for (size_t i = 0; i < inv_name.size() ; i++)
	{
		found = value.find(inv_name[i]);
		if (found != std::string::npos)
		{
	//		std::cout << red << "Found has find  in name : " << inv_name[i] << std::endl;
			return (-1);
		}	
	}
	//std::cout << "Nothing invalid for value" << std::endl;
	return (1);
}

int		Request::check_syntax_header_name(std::string & value)
{
	std::size_t found;

	for (size_t i = 0; i < inv_name.size(); i++)
	{
		found = value.find(inv_name[i]);
		if (found != std::string::npos)
		{
		//	std::cout << red << "Found has find ind value : " << inv_name[i] << std::endl;
			return (-1);
		}	
	}
	//std::cout << "Nothing invalid for name" << std::endl;
	return (1);
}

int		Request::parse_headers_cookies(std::string cookies, std::string & cookie_value)
{
	std::string tmp;
	std::vector<std::string>	cookie_list;
	std::string				cookies_tmp = cookies; 
	std::size_t				found = cookies.find(";");
	if (!cookies.empty())
		//std::cout << yellow << "Cookies line request = " << cookies << reset <<  std::endl;
	if (!cookies.empty())
	{
		while (found != std::string::npos)
		{
			tmp = cookies.substr(0,found);
			if (cookies[found + 1] == 32)
				cookies.erase(found + 1, 1);
	//		std::cout << cyan << "Tmp = " << tmp << reset << std::endl;
			cookie_list.push_back(tmp);
			cookies.erase(0,found + 1);
			found = cookies.find(";");
//			std::cout << red << "Cookie after erase = " << cookies << reset << std::endl;
		}
		tmp = cookies.substr(0,cookies.size());
//		std::cout << cyan << "Tmp = " << tmp << reset << std::endl;
		cookie_list.push_back(tmp);
		cookies.erase(0,cookies.size());
//		std::cout << red << "Cookie after erase = " << cookies << reset <<  std::endl;
		
	}
	for (std::vector<std::string>::iterator it = cookie_list.begin(); it != cookie_list.end(); it++)
	{
		//std::cout << magenta << "Cookies list : " << *it << reset << std::endl;
		found = it->find("=");
		if (found == std::string::npos)
			return (-1);
		tmp = it->substr(0,found);
		//std::cout << "Name : " << tmp << std::endl; 
		if (check_syntax_header_name(tmp) == -1)
			return (-1);
		tmp = it->substr(found + 1, it->size());
		//std::cout << "Value : " << tmp << std::endl; 
		if (check_syntax_header_value(tmp) == -1)
			return (-1);

	}
	if (!cookie_value.empty())
	{
		cookies_tmp.push_back(';');
		cookie_value += cookies_tmp;
	}
	else
		cookie_value = cookies_tmp;
	return 1;
}

int		Request::is_cookie_in_header( void )
{
	std::map<std::string, std::string> 				fields = getFields();
	std::map<std::string, std::string>::iterator	it;
	std::string										cookie_value;
	for(it = fields.begin(); it != fields.end() ;  it++)
	{
		if (it->first.compare("Cookie") == 0)
		{
			if (parse_headers_cookies(it->second, cookie_value) == -1)
				return (-1);
		}
	}
	setCookie(cookie_value);
	//std::cout << cyan << cookie_value << std::endl;
	return (1);
}

bool	Request::get_http_line(std::string& str, std::string& line) const
{
	std::string::size_type	start, end;

	if (str.empty())
		return false;
	start = 0;
	end = str.find(HTTP_ENDL);
	if (end == std::string::npos)
	{
		line = str.substr(start);
		str = "";
	}
	else
	{
		line = str.substr(start, end - start);
		str = str.substr(end + HTTP_ENDL.length());
	}
	return true;
}

bool	split_header_line(const std::string& line, std::string& var, std::string& value)
{
	std::string::size_type	pos;
	const char*	whitespaces = " \f\n\r\t\v";

	pos = line.find(":");
	if (pos == std::string::npos)
		return false;
	var = line.substr(0, pos);
	value = line.substr(pos + 1);
	for (std::string::size_type i = 0; i < var.length(); i++)
	{
		if (std::isspace(var[i]))
			return false;
	}
	value.erase(value.find_last_not_of(whitespaces) + 1);
	value.erase(0, value.find_first_not_of(whitespaces));
	return true;
}

int	Request::process_header_line(const std::string& line)
{
	std::string::size_type	pos;
	std::string	var, value;

	if (!split_header_line(line, var, value))
		return 400;
	if (var == "Host")
	{
		pos = value.find(':');
		if (pos != std::string::npos)
			value.erase(pos);
		_host = value;
	}
	else if (var == "Content-Length")
    {
        std::stringstream ss;
        ss << value;
        ss >> _content_length;
        if (ss.fail())
        {
            std::cout << "Error: stringstream parsing failed" << std::endl;
            if (!_error_code)
				_error_code = 500;
        }
    }
	else if (var == "Transfer-Encoding")
		_transfer_encoding = value;
	else if (var == "Content-Type")
		_content_type = value;
	addField(var, value);
	return 0;
}

bool    Request::is_directory( void ) const
{
    struct stat fileInfo;
    std::string path = getPathTranslated();

    if (stat(path.c_str(), &fileInfo) == 0)
	{
		if (S_ISDIR(fileInfo.st_mode))
		{
            return true;
		}
	}
    return false;
}




std::string get_remove_query(std::string & path)
{
    if (path.find('?') == std::string::npos)
        return "";
    std::string query = path.substr(path.find_last_of('?') + 1,path.size());
    if (query.size() == 0)
        return "";
    path.erase(path.find_last_of('?'),path.size());
    return query;
}

int	Request::process_req_line(std::string& line)
{
	std::string::size_type	pos;
	std::string	str;

	pos = line.find(" ");
	if (pos == std::string::npos)
		if (!_error_code)
			_error_code = 400;
	if (pos == std::string::npos){
		//std::cout << "process_req_line" << std::endl; DEBUG PURPOSE
		return 400;
	}
		
	str = line.substr(0, pos);
	if (str == "GET")
		_http_method = GET;
	else if (str == "POST")
		_http_method = POST;
	else if (str == "DELETE")
		_http_method = DELETE;
	else
		if (!_error_code)
			_error_code = 501;
	line.erase(0, pos + 1);
	pos = line.find(" ");
	if (pos == std::string::npos)
		if (!_error_code)
			_error_code = 400;
	_path = line.substr(0, pos);
    _query = get_remove_query(_path);
	line.erase(0, pos + 1);
	if (line != HTTP_VERSION)
		if (!_error_code)
			_error_code = 505;
	return 0;
}

bool		Request::isSet(const std::string& header) const
{
	std::map<std::string, std::string>::const_iterator it = _fields.find(header);
	if (it == _fields.end())
		return false;
	return true;
}

int			Request::checkHeaders(void)
{
	if (!isSet("Host") || \
	(isSet("Content-Length") && isSet("Transfer-Encoding"))){
		//std::cout << "checkheaders" << std::endl;
		return (400);
	}
	return (0);
}

int		Request::fillHeaders(std::string& to_process)
{
	size_t      endl_pos;
	std::string line;
	
	endl_pos = _fieldbuffer.find(HTTP_ENDL + HTTP_ENDL);
	if (endl_pos == std::string::npos)
	{
		endl_pos = _fieldbuffer.rfind(HTTP_ENDL);
		if (endl_pos == std::string::npos && _fieldbuffer.size() <= 8000)
			return (0);
		else if (endl_pos == std::string::npos && _fieldbuffer.size() >= 8000)
		{
			_error_code = 414;
			return (0);
		}
	}
	else
		endl_pos += 2;
	if (endl_pos != _fieldbuffer.size() - 2)
	{
		to_process = _fieldbuffer.substr(0, endl_pos + 2);
		_fieldbuffer.erase(0, endl_pos + 2);
	}
	else
	{
		to_process = _fieldbuffer;
		_fieldbuffer.clear();
	}
	if (_empty)
	{
		get_http_line(to_process, line);
		process_req_line(line);
		_empty = false;
	}
	while (get_http_line(to_process, line) && line != HTTP_ENDL && line != "")
	{
		if (int error = process_header_line(line)) {
			return (error);
		}
	}
	if (line == "")
	{
		_headerfields = 1;
		_fieldbuffer.append(to_process);
		if (checkHeaders())
			return (400);
	}
	return (1);
}

int	Request::fillBody()
{
	if (_body.size() + _fieldbuffer.size() > static_cast<unsigned int>(_config.getBody_size()))
		return (413);
	_body += _fieldbuffer;
	_body_size += _fieldbuffer.length();
	_fieldbuffer.clear();
	return (0);
}

int		Request::hexStringToInt(const std::string& hex_string) const
{
	std::istringstream 	iss(hex_string);
	int					result = 0;

	iss >> std::hex >> result;
	if (iss.fail())
		return (-1);
	return (result);
}

int		Request::fillBodyChunked(void)
{
	size_t		endl_pos;
	std::string	hex_string;

	if (_body.size() + _fieldbuffer.size() > static_cast<unsigned int>(_config.getBody_size()))
		return (413);
	while (!_fieldbuffer.empty())
	{
		if (_chunked_length == 0)
		{
			endl_pos = _fieldbuffer.find(HTTP_ENDL);
			if (endl_pos == std::string::npos)
				return (0);
			hex_string = _fieldbuffer.substr(0, endl_pos);
			if (hex_string == "0" && _fieldbuffer.substr(0, HTTP_ENDL.size() * 2 + 1) == ("0" + HTTP_ENDL + HTTP_ENDL))
			{
				_full = true;
				return (0);
			}
			_chunked_length = hexStringToInt(hex_string);
			if (_chunked_length == -1)
				return (500);
			_fieldbuffer.erase(0, endl_pos + HTTP_ENDL.size());
		}
		if ((_chunked_length) < static_cast<int>(_fieldbuffer.size()))
		{
			_body += _fieldbuffer.substr(0, _chunked_length); 
			_fieldbuffer.erase(0, _chunked_length + HTTP_ENDL.size());
			_chunked_length = 0;
		}
		else if ((_chunked_length) > static_cast<int>(_fieldbuffer.size()))
		{
			_body += _fieldbuffer;
			_chunked_length -= _fieldbuffer.size();
			_fieldbuffer.clear();
		}
	}
	return (1);
}

int    Request::addRaw(std::string str, std::vector<ServerLocation>& locations, Request& request, const int& port, const std::vector<ServerConfig>& config_list)
{
    std::string to_process;
	int			error;

	_fieldbuffer.append(str);
	if (_headerfields == false)
	{
		error = fillHeaders(to_process);
		if (_headerfields)
			match_server(locations, request, port, config_list);
		if (error != 1)
			return (error);
	}
	if (_headerfields == true)
	{
		if (!_fieldbuffer.empty() && (_content_length == 0 && _transfer_encoding != "chunked"))
		{
			_full = true;
			return 400;
		}
		if (_body_size < _content_length)
		{
			if (int error = fillBody())
			{
				_full = true;
				return (error);
			}
		}
		else if (_transfer_encoding == "chunked")
		{
			error = fillBodyChunked();
			if (error != 1)
			{
				_full = true;
				return (error);
			}
		}
		if (_body_size == _content_length && _transfer_encoding != "chunked")
		{
			_full = true;
		}
	}
	
    return (0);
}

void    Request::clear(void)
{
	_body_size = 0;
	_chunked_length = 0;
	_content_length = 0;
	_error_code = 0;
	_transfer_encoding.clear();
	_body.clear();
	_path.clear();
    _fields.clear();
    _fieldbuffer.clear();
	_host.clear();
	_content_type.clear();
	_headerfields = false;
    _empty = true;
    _full = false;
}

bool    Request::isFull(void) const
{
    return _full;
}

bool    Request::isEmpty(void) const
{
    return _empty;
}

const std::string&	Request::getQuery(void) const
{
	return _query;
}

const int&	Request::getMethod(void) const
{
	return _http_method;
}

const std::string&	Request::getPath(void) const
{
	return _path;
}

const std::string&  Request::getCookie( void ) const
{
	return _cookie;
}

void                Request::setServerLocations(const std::vector<ServerLocation>& location)
{
    _location = location;
}

void                Request::setServerConfig(const ServerConfig& config)
{
    _config = config;
}

void                Request::setPathInfo(const std::string& path_info)
{
    _path_info = path_info;
}

void                Request::setPathTranslated(const std::string& path_translated)
{
    _path_translated = path_translated;
}

void				Request::setCookie(const std::string & cookie)
{
	_cookie = cookie;
}

const std::vector<ServerLocation>&   Request::getServerLocation(void) const
{
    return _location;
}

const ServerConfig&     Request::getServerConfig(void) const
{
    return _config;
}

const std::string&      Request::getPathInfo(void) const
{
    return _path_info;
}

const std::string&      Request::getPathTranslated(void) const
{
    return _path_translated;
}

void					Request::setErrorCode(const int& error_code)
{
	_error_code = error_code;
}

int						Request::getErrorCode(void) const
{
	return _error_code;
}

bool					Request::getToClose(void) const
{
	return _to_close;
}

void					Request::setToClose(bool boolean)
{
	_to_close = boolean;
}

void match_location(std::vector<ServerLocation>& locations, const Request& request)
{
	const std::vector<ServerLocation>& configLocations = request.getServerConfig().getLocations();
    size_t lastSlashPos;
	std::string path(request.getPath());
	if (!path.empty() && path[path.length() - 1] != '/')
	{
		lastSlashPos = path.find_last_of('/');
		path = path.substr(0, lastSlashPos + 1);
	}
    while (!path.empty())
    {
        for (std::vector<ServerLocation>::const_iterator it = configLocations.begin(); it != configLocations.end(); it ++)
        {
            if ((*it).getLocation() == path)
            {
                locations.push_back(*it);
                break;
            }
        }
        path.erase(path.end() - 1);
        path = path.substr(0, path.find_last_of('/') + 1);
    }
}

void match_server(std::vector<ServerLocation>& locations, Request& request, const int& port, const std::vector<ServerConfig>& config_list)
{
	for (std::vector<ServerConfig>::const_iterator it = config_list.begin(); it != config_list.end(); it++)
	{
		std::vector<std::string> server_names = it->getServer_names();
		if (port != it->getPort())
			continue;
		for (std::vector<std::string>::const_iterator it2 = server_names.begin(); it2 != server_names.end(); it2 ++)
		{
			if (request.getHost() == *it2)
			{
				request.setServerConfig(*it);
				match_location(locations, request);
				request.setServerLocations(locations);
				return ;
			}
		}
	}
	for (std::vector<ServerConfig>::const_iterator it = config_list.begin(); it != config_list.end(); it++)
	{
		if (port == it->getPort() && it->isDefault())
		{
			request.setServerConfig(*it);
			match_location(locations, request);
			request.setServerLocations(locations);
			return ;
		}
	}
}

