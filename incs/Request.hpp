#ifndef REQUEST_HPP
# define REQUEST_HPP

# include "AHTTP.hpp"
# include "ServerConfig.hpp"

# include <sstream>
# include <sys/types.h>
# include <sys/socket.h>

const std::string red = "\033[0;31m";
const std::string green = "\033[1;32m";
const std::string yellow = "\033[1;33m";
const std::string cyan = "\033[0;36m";
const std::string magenta = "\033[0;35m";
const std::string reset = "\033[0m";
const std::string inv_name = "\"()<>@,;:\\/[]?={}";
const std::string inv_value = "\",;\\ ";
 
class Request : public AHTTP
{
	private:
		int			            			    _http_method;
		std::string	            			    _path;
        bool                   				    _empty;
        bool                    		 		_full;
		bool									_headerfields;
        std::string              		   		_fieldbuffer;
        ServerConfig                			_config;
        std::vector<ServerLocation>             _location;
		std::string								_path_info;
		std::string								_path_translated;
        std::string                             _query;
		std::string								_cookie;
		unsigned int							_body_size;
		int										_chunked_length;
		int										_error_code;
		bool									_to_close;

		bool	get_http_line(std::string& str, std::string& line) const;
		int		process_req_line(std::string& line);
		int		process_header_line(const std::string& line);

	public:
        Request();
        Request(const Request& src);
        ~Request();
        const Request&                      operator=(const Request& src);
        
		int									checkHeaders(void);
		int									fillHeaders(std::string& to_process);
		int									hexStringToInt(const std::string& hex_string) const;
		int									fillBodyChunked(void);
		int									fillBody();
        int                                 addRaw(std::string str,std::vector<ServerLocation>& locations, Request& request, const int& port, const std::vector<ServerConfig>& config_list);
        void                                clear(void);
        bool                                is_directory(void) const ;
		bool								isSet(const std::string& header) const;

		//setters
        void                                setServerConfig(const ServerConfig& config);
        void                                setServerLocations(const std::vector<ServerLocation>& location);
		void								setPathInfo(const std::string& path_info);
		void								setPathTranslated(const std::string& path_translated);
		void								setErrorCode(const int& error_code);
		void								setToClose(bool boolean);
		void								setCookie(const std::string & cookie);

		//getters
        const std::vector<ServerLocation>&              getServerLocation(void) const;
        const ServerConfig&                 			getServerConfig(void) const;
		const int&										getMethod(void) const;
		const std::string&								getPath(void) const;
		const std::string&								getPathInfo(void) const;
		const std::string&								getPathTranslated(void) const;
		const std::string&								getQuery(void) const;
		int												getErrorCode(void) const;
		bool											getToClose(void) const;
		bool                                			isEmpty(void) const;
        bool                                			isFull(void) const;
		
		const std::string&								getCookie( void ) const;
		int												is_cookie_in_header( void );
		int												parse_headers_cookies(std::string  cookies,std::string & cookie_value);
		int												check_syntax_header_name(std::string & value);
		int												check_syntax_header_value(std::string & value);



};

void	match_server(std::vector<ServerLocation>& locations, Request& request, const int& port, const std::vector<ServerConfig>& config_list);
void	match_location(std::vector<ServerLocation>& locations, const Request& request);
bool	split_header_line(const std::string& line, std::string& var, std::string& value);

#endif
