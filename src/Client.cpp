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

//handle the whole request, once it is complete
void Client::processRequest()
{
	//check if the request is valid
	if (_request->getRequestValidity() != 0)
	{
		_response->setStatusCode(_request->getRequestValidity());
		return;
	}

	//get full path and location path
	std::string full_path = _server.getRoot() + _request->getUri();

	//see if we need to decode URI before or after

	std::string location_path = _request->getUri();
	//if uri is a filename, we need to remove the filename to get the directory
	size_t pos = location_path.find_last_of('/');
	if (pos != std::string::npos)
		location_path = location_path.substr(0, pos + 1);

	//check for location properties
	for (std::vector<Location>::iterator it = _server.getLocations().begin(); it != _server.getLocations().end(); ++it)
	{
		//a location block exists, we handle it
		if (location_path == (*it).getPath())
		{
			//this is a redirection, we handle it
			if ((*it).getRedirection().empty() == false)
			{
					_response->setStatusCode(301);
					_response->setRedirection((*it).getRedirection());
					_response->setHeader("Location", (*it).getRedirection());
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
					//if yes, mark is_directory bool
					//if no, check if index file exists
						//if yes, change full path to index file
						//if no, 403 forbidden
				if ((*it).getDirectoryListing() == true)
					_request->setIsDirectory(true);
				else
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

	//check if the file exists
	if (fileExists(full_path) == false)
	{
		_response->setStatusCode(404);
		return;
	}

	//decode URI if needed (see if we dont do it first)

	//check if cgi 

	//handle methods / cgi

}