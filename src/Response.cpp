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

void Response::setIsDirectory(bool is_directory)
{
	_is_directory = is_directory;
}

void Response::setHTTPVersion(const std::string &http_version)
{
	_http_version = http_version;
}

void Response::setStatusCode(int status_code)
{
	_status_code = status_code;
}

void Response::setStatusMessage(const std::string &status_message)
{
	_status_message = status_message;
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

void Response::prepareResponse()
{
	defineContentType();
	defineStatusMessage();
	//build status line : status-line = HTTP-version SP status-code SP [ reason-phrase ]
	_buffer = "HTTP/1.1" + " " + _status_code + " " + _status_message + CRLF;

	//get right body and additionnal headers if needed (cgi / post)

	//set all necessary headers
	defineResponseHeaders();

	_buffer += _body;
}

void Response::defineContentType()
{
	//default content type
	_content_type = "text/html";

	//if path ends with an extension and not a directory we set the content type accordingly
	if (isDirectory(_full_path) == false && _full_path.find_last_of('.') != std::string::npos)
	{
		std::string extension = _full_path.substr(_full_path.find_last_of('.') + 1);
		if (extension == "html" || extension == "htm")
			_content_type = "text/html";
		else if (extension == "css")
			_content_type = "text/css";
		else if (extension == "js")
			_content_type = "text/javascript";
		else if (extension == "txt")
			_content_type = "text/plain";
		else if (extension == "xml")
			_content_type = "text/xml";
		else if (extension == "csv")
			_content_type = "text/csv";

		else if (extension == "jpg" || extension == "jpeg")
			_content_type = "image/jpeg";
		else if (extension == "png")
			_content_type = "image/png";
		else if (extension == "gif")
			_content_type = "image/gif";
		else if (extension == "bmp")
			_content_type = "image/bmp";
		else if (extension == "ico")
			_content_type = "image/x-icon";
		else if (extension == "svg")
			_content_type = "image/svg+xml";
			
		else if (extension == "mp3")
			_content_type = "audio/mpeg";
		else if (extension == "wav")
			_content_type = "audio/wav";
		else if (extension == "oga")
			_content_type = "audio/ogg";
		else if (extension == "spx")
			_content_type = "audio/ogg";
		else if (extension == "opus")
			_content_type = "audio/ogg";
		else if (extension == "mp4")
			_content_type = "video/mp4";
		else if (extension == "webm")
			_content_type = "video/webm";
		else if (extension == "ogg")
			_content_type = "video/ogg";
		else if (extension == "ogv")
			_content_type = "video/ogg";
		else if (extension == "webm")
			_content_type = "video/webm";

		else if (extension == "json")
			_content_type = "application/json";
		else if (extension == "pdf")
			_content_type = "application/pdf";
		else if (extension == "zip")
			_content_type = "application/zip";
		else if (extension == "tar")
			_content_type = "application/x-tar";
		else if (extension == "gz")
			_content_type = "application/gzip";
		else if (extension == "bz2")
			_content_type = "application/x-bzip2";
		else if (extension == "7z")
			_content_type = "application/x-7z-compressed";
		else if (extension == "rar")
			_content_type = "application/x-rar-compressed";
		else if (extension == "doc")
			_content_type = "application/msword";
		else if (extension == "docx")
			_content_type = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
		else if (extension == "xls")
			_content_type = "application/vnd.ms-excel";
		else if (extension == "xlsx")
			_content_type = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
		else if (extension == "ppt")
			_content_type = "application/vnd.ms-powerpoint";
		else if (extension == "pptx")
			_content_type = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
		else if (extension == "odt")
			_content_type = "application/vnd.oasis.opendocument.text";
		else if (extension == "ods")
			_content_type = "application/vnd.oasis.opendocument.spreadsheet";
		else if (extension == "odp")
			_content_type = "application/vnd.oasis.opendocument.presentation";
		else if (extension == "odg")
			_content_type = "application/vnd.oasis.opendocument.graphics";
		else if (extension == "odc")
			_content_type = "application/vnd.oasis.opendocument.chart";
		else if (extension == "odb")
			_content_type = "application/vnd.oasis.opendocument.database";
		else if (extension == "odf")
			_content_type = "application/vnd.oasis.opendocument.formula";
		else if (extension == "odm")
			_content_type = "application/vnd.oasis.opendocument.text-master";
		else if (extension == "ott")
			_content_type = "application/vnd.oasis.opendocument.text-template";
		else if (extension == "ots")
			_content_type = "application/vnd.oasis.opendocument.spreadsheet-template";
		else if (extension == "otp")
			_content_type = "application/vnd.oasis.opendocument.presentation-template";
		else if (extension == "otg")
			_content_type = "application/vnd.oasis.opendocument.graphics-template";
		else if (extension == "otc")
			_content_type = "application/vnd.oasis.opendocument.chart-template";
	}
}

void Response::defineStatusMessage(const int status_number)
{
	if (status_number == 200)
		_status_message = "OK";
	else if (status_number == 201)
		_status_message = "Created";
	else if (status_number == 202)
		_status_message = "Accepted";
	else if (status_number == 204)
		_status_message = "No Content";
	else if (status_number == 301)
		_status_message = "Moved Permanently";
	else if (status_number == 302)
		_status_message = "Found";
	else if (status_number == 303)
		_status_message = "See Other";
	else if (status_number == 304)
		_status_message = "Not Modified";
	else if (status_number == 400)
		_status_message = "Bad Request";
	else if (status_number == 401)
		_status_message = "Unauthorized";
	else if (status_number == 403)
		_status_message = "Forbidden";
	else if (status_number == 404)
		_status_message = "Not Found";
	else if (status_number == 405)
		_status_message = "Method Not Allowed";
	else if (status_number == 500)
		_status_message = "Internal Server Error";
	else if (status_number == 501)
		_status_message = "Not Implemented";
	else if (status_number == 502)
		_status_message = "Bad Gateway";
	else if (status_number == 503)
		_status_message = "Service Unavailable";
	else if (status_number == 504)
		_status_message = "Gateway Timeout";
	else if (status_number == 505)
		_status_message = "HTTP Version Not Supported";
	else
		_status_message = "Unknown Status";
}

void Response::defineResponseHeaders()
{
	_buffer += "Server: " + _server->getName() + CRLF;
	_buffer += "Date: " + getCurrentDate() + CRLF;
	if (_status_code == "301" || _status_code == "302")
	{
		_buffer += "Location: " + _redirection + CRLF;
		_buffer += "Content-Length: 0" + CRLF;
		_buffer += CRLF;
	}
	_buffer += "Content-Type: " + _content_type + CRLF;
	_buffer += "Content-Length: " + std::to_string(_body.size()) + CRLF;
	//_buffer += "Connection: close" + CRLF;
	_buffer += CRLF;
}