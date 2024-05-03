#include "Response.hpp"

std::string Response::itos(int integer)
{
    std::ostringstream convert;

    convert << integer;
    return convert.str();
}

int    Response::http_error_template(std::string &response, int error,  const ServerConfig & config)
{
    std::map<int, std::string> map_error = config.getErr_codes();
    std::map<int, std::string>::iterator it = map_error.begin();
    if (!map_error.empty())
    {        
        for (;it != map_error.end();it++)
        {
            if (it->first == error)
            {
                std::ifstream file(it->second.c_str());
                if (it->second.find_last_of("/") == it->second.size() -1)
                    break ;
                if (file.is_open())
                {
                    get_inside_file(response,file);
                    file.close();
                    return (error);
                }
                break ;
            }
        }
    }
	response += "<!DOCTYPE html>\n"
    "<html lang=\"en\">\n"
    "<head>\n"
    "    <meta charset=\"UTF-8\">\n"
    "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
    "    <title>Error</title>\n"
    "    <style>\n"
    "        body {\n"
    "            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;\n"
    "            background-color: #f0f3f5;\n"
    "            color: #333;\n"
    "            margin: 0;\n"
    "            padding: 0;\n"
    "            display: flex;\n"
    "            justify-content: center;\n"
    "            align-items: center;\n"
    "            height: 100vh;\n"
    "        }\n"
    "        .container {\n"
    "            max-width: 600px;\n"
    "            padding: 30px;\n"
    "            background-color: #fff;\n"
    "            border-radius: 16px;\n"
    "            box-shadow: 0 0 20px rgba(0, 0, 0, 0.1);\n"
    "            text-align: center;\n"
    "        }\n"
    "        h1 {\n"
    "            color: #d9534f;\n"
    "            margin-bottom: 20px;\n"
    "        }\n"
    "        p {\n"
    "            font-size: 18px;\n"
    "            line-height: 1.6;\n"
    "            margin-bottom: 30px;\n"
    "        }\n"
    "        footer {\n"
    "            font-size: 14px;\n"
    "            color: #666;\n"
    "            margin-top: 20px;\n"
    "        }\n"
    "    </style>\n"
    "</head>\n"
    "<body>\n"
    "    <div class=\"container\">\n"
    "        <h1>Error " + itos(error) + ": " + error_map.at(error) + "</h1>\n"
    "        <p>An error occurred while processing your request.</p>\n"
    "    </div>\n"
    "</body>\n"
    "</html>\n";
    return error;

}

std::string    Response::getRaw(const int& status) const
{
    std::string         raw;
    std::stringstream   ss;

    ss << HTTP_VERSION << " " << status << " " << error_map.at(status) << HTTP_ENDL;
    for (std::map<std::string, std::string>::const_iterator it = _fields.begin(); it != _fields.end(); it++)
        ss << it->first << ": " << it->second << HTTP_ENDL;
    ss << HTTP_ENDL << _body;
    return ss.str();
}

void			Response::clear(void)
{
		_fields.clear();
		_body.clear();
}

