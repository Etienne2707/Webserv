#include "Get.hpp"

int location_index_redirectory(ServerLocation location,std::string path,  std::string &response) // if index in conf exist try it
{
    std::ifstream file;

    if (!location.getIndex().empty())
    {
        path += location.getIndex();
        file.open(path.c_str());
        if (!file.is_open())
            return (404);
        get_inside_file(response, file);
        file.close();
        return (200);
    }
    return (404);
}

int location_auto_index_redirectory(ServerLocation location,std::string path,  std::string &response) // Auto index method 
{
    struct stat fileInfo;	            
    std::string pathent;

    if (location.getAutoindex() == true) // if auto_index enable response = list of file in auto_index directory
	{
		DIR *dir;
		struct dirent *ent;
		dir = opendir(path.c_str());
		if (dir == NULL)
			return (404);
        response += html_start;
		while ((ent = readdir(dir)) != NULL)
		{
        	response += "            <li><a href=\"";
			response += ent->d_name;
            pathent = path + ent->d_name;
            if (stat(pathent.c_str(), &fileInfo) == 0)
	        {
		        if (S_ISDIR(fileInfo.st_mode))
                    response.push_back('/');
	        }
            response += "\">";
            response += ent->d_name;
            response += "</a></li>\n";
		}
		response += "        </ul>\n"
        "    </div>\n"
        "</body>\n"
        "</html>\n";
		closedir(dir);
		return (200);
	}		
    else
		return (404);
}

void    Accept(std::string faccept, std::string &response)
{
    std::cout << faccept << std::endl;
    (void)response;
}

void    get_fields_response(std::map<std::string, std::string> fields, std::string& response,Response& get_response,Request& request) // Check header fields and add them to response when its necessary
{
    std::map<std::string, std::string>::iterator it = fields.begin();
    std::ostringstream convert;

    while(it != fields.end())
    {
        if (it->first.compare("Content-Length") == 0)
        {
            convert << response.size();
            get_response.addField("Content-Length", convert.str());
        }
        if (it->first.compare("Content-Type") == 0)
        {
            Content_Type(get_response,request);
        }
        it++;
    }
}

void	get_inside_file(std::string &result, std::ifstream &file) // met dans result tous ce qu'il y a dans un file
{
	std::string tmp;

	while(std::getline(file,tmp))
	{
		result += tmp;
		result += '\n';
	}
}



int            handle_GET(t_client& client_data, ServerLocation& location, std::string& body)
{
	std::string 	path = client_data.request.getPathTranslated();
	std::ifstream	file;
	int fd;

	if (client_data.request.is_directory() == true && path.find_last_of('/') != path.size() - 1)
			return 301;
	else if (path.find_last_of('/') == path.size() - 1)
	{
		if (location_index_redirectory(location,path,body) == 200)
		{
			client_data.request.setPathInfo(client_data.request.getPathInfo() + location.getIndex());
			return 200;
		}
		if (location_auto_index_redirectory(location,path,body) == 200)
		{
			client_data.request.setPathInfo(client_data.request.getPathInfo() + ".html");
			return 200;
		}	
		return 403;

	}
	fd = open(path.c_str(), O_RDONLY);
	if (fd == -1)
		return 404;
	file.open(path.c_str());
	get_inside_file(body,file);
	close(fd);
	file.close();
	return 200;
}