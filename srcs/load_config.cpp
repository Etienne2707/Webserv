#include "config_file.hpp"

void line_strip(std::string& line)
{
    std::string::size_type  start, end;

    start = 0;
    end = line.length();
    while (start < end && std::isspace(line[start]))
      start++;
    while (end > start && std::isspace(line[end - 1]))
      end--;
    line = line.substr(start, end - start);
}

void get_fields(t_line& line)
{
  line.field.clear();
  line.values.clear();
  line.values.str(line.str);
  if (!(line.values >> line.field))
    throw std::runtime_error("Invalid field.");
}

void prep_line(std::ifstream& file, t_line& line)
{
    std::getline(file, line.raw);
    do
    {
      line.line_nb++;
      line_strip(line.raw);
      line.is_block = false;
      line.str = line.raw;
      if (line.str[0] == '#')
        line.str.clear();
      else if (!line.str.empty())
      {
        if (line.str[0] == '[' && line.str[line.str.length() - 1] == ']')
        {
          line.is_block = true;
          line.str = line.str.substr(1, line.str.length() - 2);
          line_strip(line.str);
        }
        get_fields(line);
      }
    } while (line.str.empty() && std::getline(file, line.raw));
}


ServerLocation	read_location_block(std::ifstream& file, t_line& line)
{
	ServerLocation	location;

	location.setLocation(line.values);
	prep_line(file, line);
	while (!line.str.empty() && !line.is_block)
	{
		if (line.field == "http_methods")
			location.setHttp_methods(line.values);
		else if (line.field == "rewrite")
			location.addRewrite(line.values);
		else if (line.field == "root")
			location.setRoot(line.values);
		else if (line.field == "autoindex")
			location.setAutoindex(line.values);
		else if (line.field == "index")
			location.setIndex(line.values);
		else if (line.field == "cgi")
			location.addCgi(line.values);
		else if (line.field == "upload_store")
			location.addUpload_store(line.values);
		else
			throw std::runtime_error("Invalid location instruction.");
		prep_line(file, line);
	}
	return location;
}

ServerConfig	read_server_block(std::ifstream& file, t_line& line)
{
	ServerConfig	server;

  if (!line.is_block || line.field != "Server")
    throw std::runtime_error("All configuration must be placed inside a server block.");
  prep_line(file, line);
  while (!line.str.empty() && !(line.is_block && line.field == "Server"))
  {
    if (line.field == "port")
      server.setPort(line.values);
    else if (line.field == "server_name")
      server.addServer_names(line.values);
    else if (line.field == "err_code")
      server.addErr_code(line.values);
    else if (line.field == "body_size")
      server.setBody_size(line.values);
    else if (line.is_block && line.field == "Location")
    {
      ServerLocation  location(read_location_block(file, line));
      server.addLocation(location);
    }
    else
      throw std::runtime_error("Invalid server instruction.");

    if (!line.is_block)
      prep_line(file, line);
  }
	return server;
}

bool	load_config(const char *filename, std::vector<ServerConfig>& config)
{
	std::ifstream	file;
	t_line		line;

	file.open(filename);
	if (!file.is_open())
	{
		std::cerr << "Error: Can't open config file." << std::endl;
		file.close();
		return false;
	}
	line.line_nb = 0;
	prep_line(file, line);
	if (line.str.empty())
	{
		std::cerr << "Error: Empty config file." << std::endl;
		file.close();
		return false;
	}
	while (!line.str.empty())
	{
		try
		{
			ServerConfig  server(read_server_block(file, line));
			server.fillDefaults(config);
			config.push_back(server);
		}
		catch (const std::runtime_error& e)
		{
			std::cerr << "Error: " << e.what() << std::endl;
			if (strcmp(e.what(), "Duplicate rewrite entry found between location blocks."))
				std::cerr << "Line " << line.line_nb << ": " << line.raw << std::endl;
			file.close();
			return false;
		}
	}
	file.close();
	return true;
}
