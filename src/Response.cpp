#include <Response.hpp>

Response::Response()
{
	_is_complete = false;
}

Response::Response(const Response &response)
{
	_status_code = response._status_code;
	_status_message = response._status_message;
	_headers = response._headers;
	_body = response._body;
	_is_complete = response._is_complete;
}

Response::~Response()
{
}

Response &Response::operator=(const Response &copy)
{
	_status_code = copy._status_code;
	_status_message = copy._status_message;
	_headers = copy._headers;
	_body = copy._body;
	_is_complete = copy._is_complete;
	return *this;
}