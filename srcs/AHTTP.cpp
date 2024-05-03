#include "AHTTP.hpp"

const std::map<std::string, std::string>&	AHTTP::getFields() const
{
	return _fields;
}

bool AHTTP::getField(const std::string& field, std::string& value) const
{

	for (std::map<std::string, std::string>::const_iterator it = _fields.begin(); it != _fields.end(); it++)
	{
		if (field == it->first)
		{
			value = it->second;
			return true;
		}
	}
	return false;
}

void	AHTTP::addField(const std::string& field, const std::string& value)
{
	if (_fields.count(field))
		_fields[field] = _fields[field].append(";" + value);
	else
		_fields[field] = value;

}

const std::string&	AHTTP::getBody() const
{
	return _body;
}

void	AHTTP::setBody(const std::string& str)
{
	_body = str;
}

const std::string&	AHTTP::getHost() const
{
	return _host;
}

const unsigned int&	AHTTP::getContentLength() const
{
	return _content_length;
}

const std::string&	AHTTP::getContentType() const
{
	return _content_type;
}

const std::string&	AHTTP::getTransferEncoding() const
{
	return _transfer_encoding;
}
