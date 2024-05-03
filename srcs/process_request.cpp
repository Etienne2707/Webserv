#include "process_request.hpp"

bool isMethodAllowed(const ServerLocation& matched_location, t_client& client_data)
{
	ServerConfig	config = client_data.request.getServerConfig();
	int				method = client_data.request.getMethod();
	std::string 	content;
	std::string 	allowed_methods;
    std::string 	method_names[] = {"GET", "POST", "DELETE"};

	if (method & matched_location.getHttp_methods())
		return (true);
	int methods[] = {GET, POST, DELETE};
    for (int i = 0; i < 3; i++)
	{
     	if (matched_location.getHttp_methods() & methods[i])
         	allowed_methods.append(method_names[i] + ' ');
	}
	client_data.response.addField("Allowed", allowed_methods);
	send_response_to_client(client_data, content, 405);
	return false;
}

void    print_matching(const ServerConfig& config, const std::vector<ServerLocation>& locations) //just for testing purposes
{
    std::cout << "\nPrinting matching config found" << std::endl;
    std::cout << "\tLocations of matching config : " << std::endl;
    for (std::vector<ServerLocation>::const_iterator it = config.getLocations().begin(); it != config.getLocations().end(); it++)
		std::cout << "\t\t" << (*it).getLocation() << std::endl;
	std::cout << "\tPort of matching config : " << config.getPort() << std::endl;
	std::cout << "\tServer names of matching config : " << std::endl;
	for (std::vector<std::string>::const_iterator it = config.getServer_names().begin(); it != config.getServer_names().end(); it++)
		std::cout << "\t\t" << *it << std::endl;
	std::cout << "\tLocations config that matches request : " << std::endl;
    for (std::vector<ServerLocation>::const_iterator it = locations.begin(); it != locations.end(); it++)
		std::cout << "\t\t" << (*it).getLocation() << std::endl;
}

void	set_paths(Request& request, ServerLocation& matched_location)
{
	std::string location_name = matched_location.getLocation();
	std::string	root = matched_location.getRoot();
	std::string path = request.getPath();

	path.erase(0, location_name.length() - 1);
	request.setPathInfo(path);
	root.erase(root.end() - 1);
	request.setPathTranslated(root + path);
}

bool    is_path_accessible(t_client& client_data, ServerLocation& location)// Check if path is accesible
{
	ServerConfig	config = client_data.request.getServerConfig();
	Request			request = client_data.request;
	std::string 	path = request.getPathTranslated();
    std::size_t 	dot = path.find("./");
	std::string 	raw_response;
	Response 		response;
	std::string 	content;
	
    while (dot != std::string::npos)
    {
        if (path[dot - 1] != '.')
            path.erase(dot,2);
        else
            dot++;
        dot = path.find("./",dot);
    }
    std::size_t found = path.find("../");
    while (found != std::string::npos)
    {
        std::size_t last_slash = path.rfind("/",found - 2);
        if (last_slash != std::string::npos)
            path.erase((last_slash + 1),(found + 2) - last_slash);
        found = path.find("../");
        if (found == 1)
        {
			send_response_to_client(client_data, content, 403);
            return false;
        }
    }
    if (path.find(location.getRoot()) == std::string::npos)
    {
		send_response_to_client(client_data, content, 403);
        return false;
    }
	request.setPathTranslated(path);
    return true;
}

bool handle_rewrite(std::vector<s_rewrite> rewrite, t_client& client_data)
{
	Request				request = client_data.request;
	std::string path = 	request.getPathInfo();
	if (rewrite.empty())
		return false;
	
	for (std::vector<s_rewrite>::iterator it = rewrite.begin(); it != rewrite.end(); it++)
	{
		if (it->first.compare(path) == 0)
		{
			client_data.response.addField("Location",it->second);
			return (true);
		}
	}
	return (false);
}

void    Content_Type(Response& get_response,Request& request)
{
    std::string type;
    std::string response;
	if (request.getPathInfo().empty())
		return ;
	std::string path = request.getPathInfo().substr(1,request.getPathInfo().size());
	type = path.substr(path.rfind(".") + 1, path.size() - path.rfind("."));
    if (type.compare("html") == 0)
        response = "text/html"; // text
    else if (type.compare("css") == 0)
        response = "text/css";
    else if (type.compare("js") == 0)
        response = "text/javascript";
    else if (type.compare("jpeg") == 0)
        response = "image/jpeg"; // image
    else if (type.compare("jpg") == 0)
        response = "image/jpeg";
    else if (type.compare("png") == 0)
        response = "image/png";
    else if (type.compare("gif") == 0)
        response = "image/gif";
    else if (type.compare("pdf") == 0) // application
        response = "application/pdf";
    else if (type.compare("mp3") == 0) // audio
        response = "audio/mpeg";
    else if (type.compare("wav")== 0)
        response += "audio/wav";
    else
        response = "text/plain";
    get_response.addField("Content-Type",response);   
}

void	sendBasicErrorResponse(int error_code, t_client& client_data)
{
	std::string	body;

	send_response_to_client(client_data, body, error_code);
}

int     send_response_to_client(t_client& client_data, std::string &body, int error)
{
	ServerConfig	config = client_data.request.getServerConfig();
	

    if (error > 300)
    {
        client_data.response.http_error_template(body,error,config);
        client_data.response.addField("Content-Length", client_data.response.itos(body.size()));
    }
    else if (client_data.request.getMethod() == GET)
    {
        client_data.response.addField("Content-Length", client_data.response.itos(body.size()));
        Content_Type(client_data.response,client_data.request);
    }
    else if (client_data.request.getMethod() == DELETE)
    {
        client_data.response.addField("Content-Length", client_data.response.itos(body.size()));
    }
	else if (client_data.request.getMethod() == POST)
	{
		client_data.response.addField("Content-Length", client_data.response.itos(body.size()));
	}
	if (!body.empty())
    	client_data.response.setBody(body);
    client_data.raw_response = client_data.response.getRaw(error);
    return (error);
}   

int process_request(const std::vector<ServerConfig> &config_list, const std::string& raw_input, t_ongoing& ongoing, int packet_fd, int& epoll_fd, std::map<int, int>& server_sockets)
{
	t_client&					client_data = ongoing.requests[packet_fd];
  	std::vector<ServerLocation> locations;
	ServerLocation				matched_location;
	std::string					response_body;
	int							error_code;

	//std::cout << client_data.request.getErrorCode() << std::endl; //DEBUG PURPOSE
	//std::cout << " ----------------------RAW---------------------\n" << raw_input << "--------------------END------------------" << std::endl; //DEBUG PURPOSE
  	error_code = client_data.request.addRaw(raw_input, locations, client_data.request, client_data.port, config_list);
	if (error_code && client_data.request.getErrorCode() == 0)
		client_data.request.setErrorCode(error_code);
	if (!client_data.request.isFull())
		return (0);
	if (client_data.request.getErrorCode())
	{
		send_response_to_client(client_data, response_body, client_data.request.getErrorCode());
		if (client_data.request.getErrorCode() == 400 || client_data.request.getErrorCode() == 413 || client_data.request.getErrorCode() == 500)
		{
			client_data.response.addField("Connection", "close");
			client_data.request.setToClose(true);
		}
		return (-1);
	}
	matched_location = client_data.request.getServerLocation()[0];
	if (!isMethodAllowed(matched_location, client_data)){
		return (-1);
	}
	set_paths(client_data.request, matched_location);
	if (handle_rewrite(matched_location.getRewrite(), client_data) == true)
	{
		send_response_to_client(client_data, response_body, 301);
		return (-1);
	}
	if (is_path_accessible(client_data,matched_location) == false)
		return (-1);
	if (client_data.request.is_cookie_in_header() == -1)
	{
		client_data.response.http_error_template(response_body,400,client_data.request.getServerConfig());// How to return with -1 and shutdown the connextion;
		send_response_to_client(client_data, response_body, 400);
		return (-1);
	}
	if (!handle_cgi(client_data, client_data.request.getServerLocation(), packet_fd, epoll_fd, ongoing, server_sockets))
	{
		if (client_data.request.getMethod() == GET)
		{
			if (int error_code = handle_GET(client_data, matched_location, response_body))
				send_response_to_client(client_data, response_body, error_code);
		}
		else if (client_data.request.getMethod() == DELETE)
		{
			if (int error_code = handle_DELETE(client_data, response_body))
				send_response_to_client(client_data, response_body, error_code);
		}
		else if (client_data.request.getMethod() == POST)
		{
			if (int error_code = handle_POST(client_data, matched_location))
				send_response_to_client(client_data, response_body, error_code);
		}
	}
	client_data.request.clear();
	return (0);
}

bool	isLineEmpty(const std::string& line)
{
	if (line.empty())
		return true;
	for (std::string::size_type i = 0; i < line.size(); i++)
	{
		if (!std::isspace(line[i]))
			return false;
	}
	return true;
}

bool	process_cgi_request(t_ongoing& ongoing, int packet_fd)
{
	t_client&			client_data = ongoing.requests[ongoing.cgis[packet_fd].client_fd];
	std::string			response_body;
	std::istringstream	raw_response(ongoing.cgis[packet_fd].raw_response);
	std::string			line, var, value;
	int					error_code = 200;

	if (!std::getline(raw_response, line) || isLineEmpty(line))
		return false;
	do
	{
		if (split_header_line(line, var, value))
		{
			if (var == "Status")
			{
				std::istringstream ss(value);
				ss >> error_code;
				if (ss.fail())
					return false;
			}
			else
				client_data.response.addField(var, value);   
		}
		else
			return false;
	} while (std::getline(raw_response, line) && !isLineEmpty(line));

	std::getline(raw_response, response_body, '\0');

	send_response_to_client(client_data, response_body, error_code);

	return true;
}

void	clean_ongoing(std::map<int, int>& server_sockets, t_ongoing& ongoing, int& epoll_fd, int provenance)
{
	for (std::map<int, t_client>::iterator it = ongoing.requests.begin(); it != ongoing.requests.end(); it++)
	{
		if (provenance == IN_MAIN)
			epoll_ctl(epoll_fd, EPOLL_CTL_DEL, it->first, NULL);
		close(it->first);
	}
	for (std::map<int, t_cgi>::iterator it = ongoing.cgis.begin(); it != ongoing.cgis.end(); it++)
	{
		if (provenance == IN_MAIN)
			epoll_ctl(epoll_fd, EPOLL_CTL_DEL, it->first, NULL);
		close(it->first);
	}
	for (std::map<int, int>::iterator it = server_sockets.begin(); it != server_sockets.end(); it++)
	{
		if (provenance == IN_MAIN)
			epoll_ctl(epoll_fd, EPOLL_CTL_DEL, it->second, NULL);
		close(it->second);
	}
	close(epoll_fd);
}
