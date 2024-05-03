#ifndef CONFIG_FILE_HPP
# define CONFIG_FILE_HPP

# include <string>
# include <stdexcept>
# include <cstdlib>
# include <iostream>
# include <fstream>
# include <cstring>
# include "ServerConfig.hpp"
# include "ServerLocation.hpp"

typedef struct s_line
{
  std::string raw;
  std::string str;
  int line_nb;
  bool  is_block;
  std::string field;
  std::istringstream values;
} t_line;

bool			load_config(const char *filename, std::vector<ServerConfig>& config);
ServerConfig	read_server_block(std::ifstream& file, t_line& line);
ServerLocation	read_location_block(std::ifstream& file, t_line& line);
void			prep_line(std::ifstream& file, t_line& line);
void			get_fields(t_line& line);
void			line_strip(std::string& line);


#endif
