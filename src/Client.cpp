#include <Client.hpp>

Client::Client()
{
	_request = new Request();
	_response = new Response();

	_fd = -1;
	_cgi_pipes[0] = -1;
	_cgi_pipes[1] = -1;
}

Client::Client(int fd)
{
	_request = new Request();
	_response = new Response();

	_fd = fd;
	_cgi_pipes[0] = -1;
	_cgi_pipes[1] = -1;
}

Client::Client(const Client &client)
{
	_fd = client._fd;
	_cgi_pipes[0] = client._cgi_pipes[0];
	_cgi_pipes[1] = client._cgi_pipes[1];
	_request = client._request;
	_response = client._response;
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

void Client::readRequest(std::string &buffer)
{
	client->_request->readData(buffer);
	if (client->_request->isComplete())
		client->processRequest(server);
}

//handle the whole request, once it is complete, by filling response object and treating the request on the server side
void Client::processRequest()
{
	//set the server config in the response, so that we can access it later
	_response->setServer(_server);

	//check if the request is valid
	if (_request->getRequestValidity() != 0)
	{
		_response->setStatusCode(_request->getRequestValidity());
		return;
	}

	//get full path
	std::string full_path = _server.getRoot() + extractPathFromURI(_request->getUri());

	//check for location properties
	std::string location_path;
	size_t pos;
	for (std::vector<Location>::iterator it = _server.getLocations().begin(); it != _server.getLocations().end(); ++it)
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
			if ((*it).getRedirection().empty() == false)
			{
					_response->setStatusCode(301);
					_response->setRedirection((*it).getRedirection());
					return;
			}

			//check if the method is allowed
			if (((*it).getAllowedMethods() & _request->getMethod()) == 0)
			{
				_response->setStatusCode(405);
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
						_response->setStatusCode(403);
						return;
					}
				}
			}
			break;
		}
	}

	_respone->setFullPath(full_path);
	_response->setIsDirectory(isDirectory(full_path));

	//check if the file exists
	if (fileExists(full_path) == false)
	{
		_response->setStatusCode(404);
		return;
	}

	//see how we handle directories and default files

	//extract attributes from URI
	_response->setURIAttributes(extractAttributesFromURI(_request->getUri()));

	//specific method handlers
	if (_request->getMethod() == "GET")
		_response.handleGet();
	else if (_request->getMethod() == "POST")
		_response.handlePost();
	else if (_request->getMethod() == "DELETE")
		_response.handleDelete();
	else
		_response->setStatusCode(405);

	return;
}

void Client::sendResponse()
{
	//send the response
	//check if the response is being written, then prepare response and set it up for writing
	if (!_response->isBeingWritten())
	{
		client->_response->prepareResponse();
		client->_response->setIsBeingWritten(true);
	}

	//otherwise we keep writing until response is complete

	//when complete, we clear both request and response and reset them
}