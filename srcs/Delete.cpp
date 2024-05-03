#include "Delete.hpp"


//Need to check return code for file
int            html_delete_request(std::string &response)
{
    response +=     "<!DOCTYPE html>\n"
    "<html lang=\"en\">\n"
    "<head>\n"
    "    <meta charset=\"UTF-8\">\n"
    "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
    "    <title>Delete Successful</title>\n"
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
    "            color: green;\n"
    "            margin-bottom: 20px;\n"
    "        }\n"
    "        p {\n"
    "            font-size: 18px;\n"
    "            line-height: 1.6;\n"
    "            margin-bottom: 30px;\n"
    "        }\n"
    "        footer {\n"
    "            font-size: 14px;\n"
    "            color: green;\n"
    "            margin-top: 20px;\n"
    "        }\n"
    "    </style>\n"
    "</head>\n"
    "<body>\n"
    "    <div class=\"container\">\n"
    "        <h1>Delete Successful</h1>\n"
    "        <p>The resource has been successfully deleted.</p>\n"
    "    </div>\n"
    "</body>\n"
    "</html>\n";
	
    return (200);
}

int            handle_DELETE(t_client& client_data, std::string& body)
{
	Request			request = client_data.request;
	ServerConfig	config = request.getServerConfig();
    std::string 	path = request.getPathTranslated();

    if (request.is_directory() == true && path.find_last_of('/') != path.size() - 1)
    	return 409;
    else if (request.is_directory() == true || path.find_last_of('/') == path.size() - 1)
    {
        if (access(path.c_str(),W_OK) != 0)
            return 403;
        if (std::remove(path.c_str()) == 0)
        {
            html_delete_request(body);
            return 200;
        }
        else
            return 500;
    }
    if (std::remove(path.c_str()) != 0)
        return 403;
    html_delete_request(body);
    return 200;

}