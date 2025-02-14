#include <Client.hpp>

Client::Client()
{
	_request = new Request();
	_response = new Response();

	_fd = -1;
	_cgi_pipes[0] = -1;
	_cgi_pipes[1] = -1;
}

Client::Client(int fd, struct sockaddr_in addr, Server *server)
{
	_request = new Request();
	_response = new Response();

	_fd = fd;
	_addr = addr;
	_cgi_pipes[0] = -1;
	_cgi_pipes[1] = -1;
	_server = server;
}

Client::Client(const Client &client)
{
	_fd = client._fd;
	_cgi_pipes[0] = client._cgi_pipes[0];
	_cgi_pipes[1] = client._cgi_pipes[1];
	_request = new Request(*(client._request));
	_response = new Response(*(client._response));
	_server = client._server;
	_addr = client._addr;
}

Client::~Client()
{
	delete _request;
	delete _response;
}

Client &Client::operator=(const Client &copy)
{
	delete _request;
	delete _response;

	_request = new Request(*(copy._request));
	_response = new Response(*(copy._response));
	_fd = copy._fd;
	_cgi_pipes[0] = copy._cgi_pipes[0];
	_cgi_pipes[1] = copy._cgi_pipes[1];
	_server = copy._server;
	_addr = copy._addr;
	return *this;
}

bool Client::operator==(const Client &copy) const
{
	return (_fd == copy._fd);
}

void Client::setFd(const int fd)
{
	_fd = fd;
}

void Client::setServer(Server *server)
{
	_server = server;
}

struct sockaddr_in Client::getAddr()
{
	return _addr;
}

int Client::getFd()
{
	return _fd;
}

Request *Client::getRequest()
{
	return _request;
}

Response *Client::getResponse()
{
	return _response;
}

Server *Client::getServer()
{
	return _server;
}

void Client::readRequest()
{
	char buffer[BUFFER_SIZE + 1];
	int bytes_received;
	int client_fd = _fd;

	bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);
	if (bytes_received < 0)
	{
		FD_CLR(client_fd, &current_fds);
		if (close(client_fd) < 0)
			throw std::runtime_error("Error: Could not close socket");
		fd_to_sockfd.erase(client_fd);
		return;
	}
	else if (bytes_received == 0)
	{
		FD_CLR(client_fd, &current_fds);
		if (close(client_fd) < 0)
			throw std::runtime_error("Error: Could not close socket");
		std::cout << GREEN << "Client " << client_fd << " : disconnected" << RESET << std::endl;
		fd_to_sockfd.erase(client_fd);
		return;
	}

	buffer[bytes_received] = '\0';
	if (!_request->isComplete())
	{
		_request->readData(buffer);
		if (_request->isComplete())
		{
			//debug stuff
				std::cout << "--- Request received ---" << std::endl;
				std::cout << "Raw buffer : " << _request->getBuffer() << std::endl;
				std::cout << "Full buffer : " << _request->getBuffer() << std::endl;
				std::cout << "Method : " << _request->getMethod() << std::endl;
				std::cout << "URI : " << _request->getUri() << std::endl;
				std::cout << "HTTP version : " << _request->getHttpVersion() << std::endl;
				std::cout << "Body : " << _request->getBody() << std::endl;
				std::cout << "Headers : " << std::endl;
				for (std::map<std::string, std::string>::iterator it = _request->getHeaders().begin(); it != _request->getHeaders().end(); ++it)
					std::cout << it->first << ": " << it->second << std::endl;
				std::cout << "Request validity : " << _request->getRequestValidity() << std::endl;
				std::cout << "--- End of request, processing it ---" << std::endl;

			processRequest();
		}
	}
}

//handle the whole request, once it is complete, by filling response object and treating the request on the server side
void Client::processRequest()
{
	//set the server config in the response, so that we can access it later
	_response->setServer(_server);

	//check if the request is valid
	if (_request->getRequestValidity() != 0)
	{
		std::string error_number = std::to_string(_request->getRequestValidity());
		_response->setStatusCode(error_number);
		return;
	}

	//get full path
	std::string root_path = _server->getRoot();
	std::string extracted = extractPathFromURI(_request->getUri());
	if (root_path.back() == '/' && extracted.front() == '/')
		extracted.erase(extracted.begin());
	else if (root_path.back() != '/' && extracted.front() != '/')
		extracted = "/" + extracted;
	std::string full_path  = root_path + extracted;


	//debug
	std::cout << "Full path : " << full_path << std::endl;

	//check for location properties
	std::string location_path;
	size_t pos;

	for (std::vector<Location>::iterator it = _server->getLocations().begin(); it != _server->getLocations().end(); ++it)
	{
		location_path = extractPathFromURI(_request->getUri());

		//we check if location value ends with a /, location is a directory
		pos = location_path.find_last_of('/');
			if (pos != std::string::npos)
				location_path = location_path.substr(0, pos + 1);
		if ((*it).getPath().back() == '/' && location_path.back() != '/')
			location_path += "/";
		else if ((*it).getPath().back() != '/' && location_path.back() == '/')
			location_path.pop_back();

		//a location block exists, we handle it
		if (location_path == (*it).getPath())
		{
			//this is a redirection, we handle it
			if ((*it).getRedirect().empty() == false)
			{
					_response->setStatusCode("301");
					_response->setRedirection((*it).getRedirect());
					return;
			}

			//check if the method is allowed
			if (((*it).getAllowedMethods() & _request->getMethodBit()) == 0)
			{
				_response->setStatusCode("405");
				return;
			}

			//check if the request is a directory
			if (isDirectory(full_path))
			{
				//if yes, check if directory listing is allowed
					//if no, check if index file exists
						//if yes, change full path to index file
						//if no, 403 forbidden
				if ((*it).getDirectoryListing() == false)
				{
					if ((*it).getIndex().empty() == false)
						full_path += (*it).getIndex();
					else
					{
						_response->setStatusCode("403");
						return;
					}
				}
			}
			break;
		}
	}

	_response->setFullPath(full_path);
	_response->setIsDirectory(isDirectory(full_path));

	//check if the file exists
	if (checkPathExists(full_path) == false)
	{
		_response->setStatusCode("404");
		return;
	}

	//extract attributes from URI
	_response->setURIAttributes(extractAttributesFromURI(_request->getUri()));

	//specific method handlers
	if (_request->getMethod() == "GET")
		_response->handleGET();
	else if (_request->getMethod() == "POST")
		_response->handlePOST();
	else if (_request->getMethod() == "DELETE")
		_response->handleDELETE();
	else
		_response->setStatusCode("405");

	return;
}

void Client::sendResponse()
{
	int	send_ret = 0;

	//check if the response is being written, then prepare response and set it up for writing
	if (!_response->is_being_written)
	{
		_response->prepareResponse();
		_response->is_being_written = true;


		//print response for debug
			std::cout << "--- Response generated---" << std::endl;
			std::cout << "Status code : " << _response->getStatusCode() << std::endl;
			std::cout << "Status message : " << _response->getStatusMessage() << std::endl;
			std::cout << "Headers : " << std::endl;
			std::cout << _response->getHeaders() << std::endl;
			std::cout << "Body : " << std::endl;
			std::cout << _response->getBody() << std::endl;
			std::cout << "Full path : " << _response->getFullPath() << std::endl;
			std::cout << "URI attributes : " << _response->getURIAttributes() << std::endl;
			std::cout << "Redirection : " << _response->getRedirection() << std::endl;
			std::cout << "Content type : " << _response->getContentType() << std::endl;
			std::cout << "HTTP version : " << _response->getHTTPVersion() << std::endl;
			std::cout << "Is directory : " << _response->getIsDirectory() << std::endl;
	}

	//we write the response
	if (_response->is_being_written == true)
	{
		if (_response->getBuffer().length() <= BUFFER_SIZE)
			send_ret = write(_fd, _response->getBuffer().c_str(), _response->getBuffer().length());
		else
			send_ret = write(_fd, _response->getBuffer().c_str(), BUFFER_SIZE);
	}

	//we check return of write
	if (send_ret < 0)
	{
		FD_CLR(_fd, &current_fds);
		if (close(_fd) < 0)
			throw std::runtime_error("Error: Could not close socket");
		std::cout << RED << "Client " << _fd << " : disconnected" << RESET << std::endl;
		_server->clients.erase(std::remove(_server->clients.begin(), _server->clients.end(), *this), _server->clients.end());
		fd_to_sockfd.erase(_fd);
		return;
	}
	else if (send_ret == 0 || _response->getBuffer().length() == static_cast<size_t>(send_ret))
	{
		_response->is_being_written = false;
		_response->setBuffer("");
	}
	else
		_response->setBuffer(_response->getBuffer().substr(send_ret));

	//if response is complete, we reset the response and request
	if (_response->is_being_written == false)
	{
		if (_response->getStatusCode() == "400" || _response->getStatusCode() == "500" || _response->getHeaders("Connection") == "close")
		{
			FD_CLR(_fd, &current_fds);
			if (close(_fd) < 0)
				throw std::runtime_error("Error: Could not close socket");
			std::cout << GREEN << "Client " << _fd << " : disconnected" << RESET << std::endl;
			_server->clients.erase(std::remove(_server->clients.begin(), _server->clients.end(), *this), _server->clients.end());
			fd_to_sockfd.erase(_fd);
		}
		else
		{
			_request->resetRequest();
			_response->resetResponse();
		}
	}
}