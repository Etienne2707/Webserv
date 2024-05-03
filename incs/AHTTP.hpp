#ifndef AHTTP_HPP
# define AHTTP_HPP

# include <string>
# include <map>
# include <stdexcept>
# include <fcntl.h>
# include <fstream>
# include <unistd.h>
# include <iostream>
# include <sys/types.h>
# include <dirent.h>
# include <sstream>
# include <sys/stat.h>


#define SSTR( x ) static_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()

extern std::map<int, std::string> error_map;
const std::string HTTP_VERSION = "HTTP/1.1";
const std::string HTTP_ENDL = "\r\n";

class AHTTP
{
	protected:
		std::map<std::string, std::string>	_fields;
		std::string	                        _body;
		std::string	                        _host;
		unsigned int						_content_length;
		std::string							_content_type;
		std::string							_transfer_encoding;

	public:
		const std::map<std::string, std::string>&	getFields() const;
		bool                                        getField(const std::string& field, std::string& value) const;
		void	                                    addField(const std::string& field, const std::string& value);

		const std::string&	                        getBody() const;
		void	                                    setBody(const std::string& str);

		const std::string&	                        getHost() const;
		const unsigned int&							getContentLength() const;
		const std::string&							getContentType() const;
		const std::string&							getTransferEncoding() const;
};

#endif
