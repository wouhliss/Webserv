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

void Response::setRedirection(const std::string &redirection)
{
	_redirection = redirection;
}

void Response::setFullPath(const std::string &full_path)
{
	_full_path = full_path;
}

void Response::setURIAttributes(const std::string &uri_attributes)
{
	_uri_attributes = uri_attributes;
}

void Response::setServer(Server *server)
{
	_server = server;
}

//specific handlers for methods
//assume that the request is valid and has been correctly parsed
void Response::handleGET()
{
	if (_is_directory == true)
	{
		//function to generate directory listing
	}
	else
	{
		//handle file serving
	}
}

void Response::handleDELETE()
{
	//check for authorization header (403 otherwise)
	//remove the file
	//500 if error, 204 if success
	if (remove(_full_path.c_str()) != 0)
	{
		_status_code = "500";
		_status_message = "Internal Server Error";
	}
	else
	{
		_status_code = "204";
		_status_message = "No Content";
	}
}

void Response::handlePOST()
{
	//handle the post request
}