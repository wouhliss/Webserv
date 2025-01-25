#include <Request.hpp>

Request::Request()
{
	_is_complete = false;
	_is_valid = 0;
}

Request::Request(const Request &request)
{
	_buffer = request._buffer;
	_last_line = request._last_line;
	_method = request._method;
	_uri = request._uri;
	_http_version = request._http_version;
	_headers = request._headers;
	_body = request._body;
	_is_complete = request._is_complete;
	_is_valid = request._is_valid;
}

Request::~Request()
{
}

Request &Request::operator=(const Request &copy)
{
	_buffer = copy._buffer;
	_last_line = copy._last_line;
	_method = copy._method;
	_uri = copy._uri;
	_http_version = copy._http_version;
	_headers = copy._headers;
	_body = copy._body;
	_is_complete = copy._is_complete;
	_is_valid = copy._is_valid;
	return *this;
}

//getters

const std::string &Request::getBuffer(void) const
{
	return _buffer;
}

const std::string &Request::getLastLine(void) const
{
	return _last_line;
}

const std::string &Request::getMethod(void) const
{
	return _method;
}

const std::string &Request::getUri(void) const
{
	return _uri;
}

const std::string &Request::getHttpVersion(void) const
{
	return _http_version;
}

const std::map<std::string, std::string> &Request::getHeaders(void) const
{
	return _headers;
}

const std::string &Request::getBody(void) const
{
	return _body;
}

//setters


void Request::setMethod(const std::string &method)
{
	_method = method;
}

void Request::setUri(const std::string &uri)
{
	_uri = uri;
}

void Request::setHttpVersion(const std::string &http_version)
{
	_http_version = http_version;
}

void Request::setHeaders(const std::map<std::string, std::string> &headers)
{
	_headers = headers;
}

void Request::setBody(const std::string &body)
{
	_body = body;
}


void Request::setRequestValidity(int value, bool is_complete)
{
	_is_valid = value;
	_is_complete = is_complete;
}

//parse the first line of the request
//A request-line begins with a method token, followed by a single space (SP), the request-target, and another single space (SP), and ends with the protocol version.
bool Request::parseFirstLine(std::string line)
{
	size_t pos, pos2 = 0;

	//get the method
	pos = line.find(' ');
	if (pos == std::string::npos)
		return false;
	//check if the method is valid
	if (line.substr(0, pos) != "GET" && line.substr(0, pos) != "POST" && line.substr(0, pos) != "DELETE")
		return false;
	_method = line.substr(0, pos);

	//get the uri
	pos2 = line.find(' ', pos + 1);
	if (pos2 == std::string::npos)
		return false;
	_uri = line.substr(pos + 1, pos2 - pos - 1);

	//check if http version is valid and exists
	if (line.substr(pos2 + 1) != "HTTP/1.1")
		return false;
	_http_version = line.substr(pos2 + 1);

	return true;
}

//Adds new read data to the existing request
void Request::appendData(std::string data)
{
	//if buffer is empty and data is newline, do nothing
	if (_buffer.empty() && data == NEWLINE)
		return;
	//if buffer is empty and data is empty and request does not contain CRLF, request is invalid
	if (_last_line.empty() && data.empty() && _request.find(CRLF) == std::string::npos)
	{
		setRequestValidity(HTTP_ERROR_BAD_REQUEST, true);
		return;
	}

	//append remainder of the last line to the data
	data = _last_line + data;
	_last_line.clear();

	size_t pos = 0;
	//we read data line by line, demarcated by \r\n
	while ((pos = data.find(NEWLINE)) != std::string::npos)
	{
		//if the request buffer is empty, we parse the first line of the request
		if (_buffer.empty())
		{
			if (pos != 0 && parseFirstLine(data.substr(0, pos)) == false)
			{
				setRequestValidity(HTTP_ERROR_BAD_REQUEST, true);
				return;
			}
		}

		//otherwise, parse headers

		//we trim the beginning of the line and continue the loop
		data = data.substr(pos + 2);
	}
	
	//otherwise we store the remainder of the data in the last line
	_last_line = data;
	return ;
}