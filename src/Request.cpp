#include <Request.hpp>

Request::Request()
{
	_is_complete = false;
	_is_valid = false;
}

Request::Request(const Request &request)
{
	_buffer = request._buffer;
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

