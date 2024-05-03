#ifndef SERVERLOCATION_HPP
# define SERVERLOCATION_HPP

# include <string>
# include <sstream>
# include <map>
# include <exception>
# include <vector>
# include <algorithm>

# define POST 1
# define GET 2
# define DELETE 4

typedef struct s_rewrite
{
	std::string	first;
	std::string	second;
}	t_rewrite;

class	ServerLocation
{
	private:
		std::string		                    _location;
		int				                    _http_methods;
		std::vector<t_rewrite>				_rewrite;
		std::string		                    _root;
		int			                        _autoindex;
		std::string		                    _index;
		std::map<std::string, std::string>	_cgi;
		std::string                         _upload_store;

		void  fillDefaultRoot(const ServerLocation& location);
		void  fillDefaultAutoindex(const ServerLocation& location);
		void  fillDefaultRewrite(const std::vector<ServerLocation>& locations);

	public:
		ServerLocation();
		~ServerLocation();
		ServerLocation(const ServerLocation& old);
		ServerLocation& operator=(const ServerLocation& old);

		const std::string&							getLocation() const;
		const int&									getHttp_methods() const;
		const std::vector<t_rewrite>&				getRewrite() const;
		const std::string&							getRoot() const;
		bool										getAutoindex() const;
		const std::string&							getIndex() const;
		const std::map<std::string, std::string>&	getCgi() const;
		const std::string&							getUpload_store() const;

		void	setLocation(std::istringstream& location);
		void	setHttp_methods(std::istringstream& http_methods);
		void	addRewrite(std::istringstream& rewrite);
		void	setRoot(std::istringstream& root);
		void	setAutoindex(std::istringstream& autoindex);
		void	setIndex(std::istringstream& index);
		void	addCgi(std::istringstream& cgi);
		void	addUpload_store(std::istringstream& upload_store);
		void	fillDefaults(const std::vector<ServerLocation>& locations);


};

bool	is_path_valid(std::string& path);
void	get_inside_file(std::string &result, std::ifstream &file);

#endif
