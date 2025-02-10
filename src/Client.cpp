#include <Client.hpp>

Client::Client()
{
	_request = new Request();
	_response = new Response();

	_fd = -1;
	_cgi_pipes[0] = -1;
	_cgi_pipes[1] = -1;
}

Client::Client(int fd, struct sockaddr_in addr)
{
	_request = new Request();
	_response = new Response();

	_fd = fd;
	_addr = addr;
	_cgi_pipes[0] = -1;
	_cgi_pipes[1] = -1;
}

Client::Client(const Client &client)
{
	_fd = client._fd;
	_cgi_pipes[0] = client._cgi_pipes[0];
	_cgi_pipes[1] = client._cgi_pipes[1];
	_request = new Request(*(client._request));
	_response = new Response(*(client._response));
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
	return *this;
}

void Client::setFd(const int fd)
{
	_fd = fd;
}

void Client::setServer(Server *server)
{
	_server = server;
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
		std::cout << "Client " << client_fd << " disconnected" << std::endl;
		fd_to_sockfd.erase(client_fd);
		return;
	}

	buffer[bytes_received] = '\0';
	// if (_request->isComplete())
	// {
	// 	_request->readData(buffer);
	// 	if (_request->isComplete())
	// 		processRequest();
	// }
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
	std::string full_path = _server->getRoot() + extractPathFromURI(_request->getUri());

	//check for location properties
	std::string location_path;
	size_t pos;
	for (std::vector<Location>::iterator it = _server->getLocations().begin(); it != _server->getLocations().end(); ++it)
	{
		location_path = extractPathFromURI(_request->getUri());
		//we check if location value ends with a /, if yes we only keep the last / of location_path
		if ((*it).getPath().back() == '/')
		{
			pos = location_path.find_last_of('/');
			if (pos != std::string::npos)
				location_path = location_path.substr(0, pos + 1);
		}


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

	//see how we handle directories and default files

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
	//send the response
	//check if the response is being written, then prepare response and set it up for writing
	if (!_response->is_being_written)
	{
		_response->prepareResponse();
		_response->is_being_written = true;
	}

	if (_response->is_being_written == true)
	{
		//write response
		//if write is complete, we clear the response and request and reset them
		//if not, we keep writing until response is complete
	}
}