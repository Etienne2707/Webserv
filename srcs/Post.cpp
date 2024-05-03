#include "Post.hpp"

static int     set_upload_store(std::string& upload_store, const ServerLocation& matched_location)
{
    std::string	root;

	upload_store = matched_location.getUpload_store();
	root = matched_location.getRoot();
	if (upload_store.empty())
	{
		std::cout << "Location does not support upload" << std::endl;
		return (405);
	}
	if (upload_store[0] != '/')
		upload_store = root + upload_store;
	if ((upload_store[upload_store.size() - 1]) != '/')
	{
		std::cout << "Upload store is not a path" << std::endl;
		return (500);
	}
	return (0);
}

static int  get_boundary(std::string& header_value, std::string& boundary)
{
    size_t pos = header_value.find("boundary");

    if (pos == std::string::npos)
    {
        std::cout << "Error: Boundary not found in multipart/form-data" << std::endl;;
        return (400);
    }
    boundary = "--";
    boundary = boundary + header_value.substr(pos + 9);
	return (0);
}

bool	get_part_line(std::string& str, std::string& line)
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

int	process_part_line(std::string& line, Request& request)
{
	std::string::size_type	pos;
	std::string	str;

	pos = line.find(": ");
	if (pos == std::string::npos){
		std::cout << "process_part_line" << std::endl; //DEBUG PURPOSE
		return (400);
	}
	str = line.substr(0, pos);
	line.erase(0, pos + 2);
	request.addField(str, line);
    return 0;
}

static int  split_multipart_body(const std::string& body, const std::string& boundary, std::vector<Request>& parts)
{
    std::string str(body);
    std::string line;
    Request     request;
	size_t		boundary_pos;
	std::string part_body;
	bool		reach_end(false);

    get_part_line(str, line);
    if (line != (boundary))
    {
        std::cout << "Form format is wrong" << std::endl; //DEBUG PURPOSE
        return (400);
	}
    while (reach_end == false)
    {
        request.clear();
        while (get_part_line(str, line) && line != HTTP_ENDL && line != "" && line != (boundary))
		{
            process_part_line(line, request);
		}
		boundary_pos = str.find(HTTP_ENDL + boundary);
        if (line == "" && boundary_pos != std::string::npos)
        {
			part_body = str.substr(0, boundary_pos);
            request.setBody(part_body);
			str.erase(0,boundary_pos + boundary.length() + HTTP_ENDL.length() * 2);
			parts.push_back(request);
        }
		else if (boundary_pos == std::string::npos)
			reach_end = true;

    }
    return (0);
}

int	saveFile(const std::string& filename, const std::string& content)
{
	std::ofstream outputFile(filename.c_str(), std::ios::binary);
	if (!outputFile.is_open())
	{
		std::cout << "Error: unable to open file for writing: " << filename << std::endl; //DEBUG PURPOSE
		return 500;
	}
	outputFile.write(content.c_str(), content.length());
	outputFile.close();
	if (!outputFile.good())
	{
		std::cout << "Error: failed to write file: " << filename << std::endl; //DEBUG PURPOSE
		return 500;
	}
	else
		std::cout << "File saved successfully: " << filename << std::endl; //DEBUG PURPOSE
	return (0);
}

static int  handle_parts(const std::vector<Request>& parts, std::string& path)
{
    for (std::vector<Request>::const_iterator it = parts.begin(); it != parts.end(); it++)
    {
        std::string                                     content_disposition;
        std::map<std::string, std::string>              fields = (*it).getFields();
        std::map<std::string, std::string>::iterator    fields_it = fields.find("Content-Disposition");
        std::string                                     filename;
		std::string										content((*it).getBody());
		size_t											pos;

        if (fields_it == fields.end())
        {
            std::cout << "Content-Disposition not found in part" << std::endl; //DEBUG PURPOSE
            return (400);
        }
        content_disposition = fields_it->second;
		pos = content_disposition.find("filename=\"");
        if (pos != std::string::npos)
        {
			pos += 10;
			size_t	quote_pos = content_disposition.find("\"", pos);
			if (quote_pos == std::string::npos)
			{
				std::cout << "Filename not in double quotes" << std::endl; //DEBUG PURPOSE
				return (400);
			}
			filename = content_disposition.substr(pos, quote_pos - pos);
			if (int error = saveFile(path + filename, content))
				return (error);
        }
    }
	return (0);
}

int    handle_POST(t_client& client_data, const ServerLocation& matched_location)
{
	ServerConfig										config = client_data.request.getServerConfig();
    std::map<std::string, std::string>                 	fields = client_data.request.getFields();
    std::map<std::string, std::string>::iterator		it = fields.find("Content-Type");
    std::string                                        	content_type;
    std::string                                        	boundary;
	std::string											upload_store;
    std::vector<Request>                                parts;
	std::string											body;


	(void)config;
    if (it == fields.end())
    {
        std::cout << "Content-Type header not found" << std::endl; //DEBUG PURPOSE
        return (400);
    }
	if (int error = set_upload_store(upload_store, matched_location))
        return (error);
    size_t    pos = it->second.find(";");
    if (pos != std::string::npos)
		content_type = it->second.substr(0, pos);
	else
		content_type = it->second;
    if (content_type == "multipart/form-data" && (client_data.request.getContentLength() > 0 || client_data.request.getTransferEncoding() == "chunked"))
    {
        if (int error = get_boundary(it->second, boundary))
			return (error);
        if (int error = split_multipart_body(client_data.request.getBody(), boundary, parts))
			return (error);
		if (int error = handle_parts(parts, upload_store))
			return (error);
		send_response_to_client(client_data, body, 201);
    }
	else
	{
		std::cout << "Content-Type is not multipart/form-data for POST request" << std::endl; //DEBUG PURPOSE
		return (415);
	}
	return (0);
}
