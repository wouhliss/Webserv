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
	//if request is ready or invalid, we process it
	if (client->_request->isComplete())
	{
		if (client->_request->isValid())
			client->processRequest();
		else
			client->processErrorRequest(client->_request->getRequestValidity());
	}
}

//handle the whole request, once it is complete
void Client::processRequest()
{

}

//error on request, we send error and reset current request
void Client::processErrorRequest(int error_code)
{
	
}