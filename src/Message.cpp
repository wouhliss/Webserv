#include "../inc/Message.hpp"

Message::Message()
{
	_type = REQUEST;
	_method = "";
	_request_target = "";
	_http_version = "";
	_status_code = "";
	_status_message = "";
}

Message::Message(const Message& copy)
{
	*this = copy;
}

Message::~Message()
{
}

Message& Message::operator=(const Message &copy)
{
	if (this == &copy)
		return (*this);
	_type = copy._type;
	_method = copy._method;
	_request_target = copy._request_target;
	_http_version = copy._http_version;
	_status_code = copy._status_code;
	_status_message = copy._status_message;
	_headers = copy._headers;
	_body = copy._body;
	return (*this);
}


//Getters and Setters

void	Message::setType(bool type)
{
	_type = type;
}

void	Message::setMethod(string method)
{
	_method = method;
}

void	Message::setRequestTarget(string request_target)
{
	_request_target = request_target;
}

void	Message::setHttpVersion(string http_version)
{
	_http_version = http_version;
}

void	Message::setStatusCode(string status_code)
{
	_status_code = status_code;
}

void	Message::setStatusMessage(string status_message)
{
	_status_message = status_message;
}

void	Message::addHeader(string key, string value)
{
	_headers[key] = value;
}

void	Message::setBody(string body)
{
	_body = body;
}

bool&						Message::getType()
{
	return (_type);
}

string&						Message::getMethod()
{
	return (_method);
}

string&						Message::getRequestTarget()
{
	return (_request_target);
}

string&						Message::getHttpVersion()
{
	return (_http_version);
}

string&						Message::getStatusCode()
{
	return (_status_code);
}

string&						Message::getStatusMessage()
{
	return (_status_message);
}

std::map<string, string>&	Message::getHeaders()
{
	return (_headers);
}

string&						Message::getBody()
{
	return (_body);
}

//Private methods

bool	Message::_isValidRequest()
{
	return (true);
}