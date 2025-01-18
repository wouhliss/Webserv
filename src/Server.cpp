#include "../inc/Server.hpp"

Server::Server()
{
}

Server::Server(ConfigServer &server)
{

	char cwd[PATH_MAX];

	_server_name = server.getServerName();
	_root = std::string(getcwd(cwd, sizeof(cwd))) + "/" + server.getRoot();

	_port = server.getPort();
	_addr.sin_family = AF_INET;
	_addr.sin_port = htons(_port);
	_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	_addrlen = sizeof(_addr);

	_max_body_size = server.getSize();
	_default_file = server.getDefaultFile();
	_locations = server.getLocations();
	_error_pages = server.getErrorPages();
	// _cookies = server.getCookies();

	// Init response codes here

	_init_sockets();
}

Server::Server(const Server &copy)
{
	*this = copy;
}

Server::~Server()
{
	for (std::map<int, int>::iterator it = fd_to_sock.begin(); it != fd_to_sock.end(); ++it)
	{
		if (it->second == _socketfd)
		{
			close(it->first);
		}
	}
	close(_socketfd);
}

Server &Server::operator=(const Server &copy)
{
	if (this != &copy)
	{
		_port = copy._port;
		_server_name = copy._server_name;
		_root = copy._root;
		_max_body_size = copy._max_body_size;
		_default_file = copy._default_file;
		_locations = copy._locations;
		_error_pages = copy._error_pages;
		// _cookies = copy._cookies;
	}
	return (*this);
}

// Main function launched at startup, that will init all the sockets
/*
Order of functions :
1. Call socket to create a new socket
2. Call setsockopt to set the socket options
	- SOL_SOCKET : level of the socket option we want to set
	- SO_REUSEADDR : allow to reuse the address
	- set it to on
3. Call fcntl to set the socket to non-blocking
4. call bind to bind the socket to the address and port number
5. call listen to listen for incoming connections
*/
void Server::_init_sockets()
{
	// Create a new socket
	if ((_socketfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		std::cerr << "Error creating socket" << std::endl;
		exit(1);
	}

	// Set the socket options
	int opt = 1;
	if (setsockopt(_socketfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		close(_socketfd);
		std::cerr << "Error setting socket options" << std::endl;
		exit(1);
	}

	max_fd = _socketfd;

	sock_fd[_socketfd] = true;

	FD_SET(_socketfd, &currentfds);

	// Bind the socket to the address and port number
	if (bind(_socketfd, (struct sockaddr *)&_addr, _addrlen) < 0)
	{
		close(_socketfd);
		std::cerr << "Error binding socket to address and port number" << std::endl;
		exit(1);
	}

	// Listen for incoming connections
	if (listen(_socketfd, 10) < 0)
	{
		close(_socketfd);
		std::cerr << "Error listening for incoming connections" << std::endl;
		exit(1);
	}
}

// main fetch function that will be inside in the main loop
// uses select to check for incoming connections and treat them accordingly
// FROM HERE we want resilience, we cannot exit on errors, we have to handle them and continue
// void Server::fetch()
// {

// }

// Function to handle incoming requests

// // add error handling: for CGI we have to break at the cgi delimiter if found
// void Server::_handle_request(int fd)
// {
// }

// // Function to handle response messages

// void Server::_handle_response(int fd)
// {
// 	// we check if the fd is in the map, if not we return
// 	if (_request_buffer.find(fd) == _request_buffer.end())
// 		return;

// 	Message request = messageParser::parseMessage(_request_buffer[fd]);
// 	// we check if the request is a valid request
// 	if (request._isValidRequest())
// 	{
// then we handle the request
// _treatRequest(request);

// we write an answer based on the method invoqued
// 	}
// }

// void Server::_treatRequest(Message &request, int fd)
// {
// 	std::string method = request.getMethod();
// 	if (method == "GET")
// 	{
// 		// _handleGetRequest(request, fd);
// 	}
// 	else if (method == "POST")
// 	{
// 		// _handlePostRequest(request, fd);
// 	}
// 	else if (method == "DELETE")
// 	{
// 		// _handleDeleteRequest(request, fd);
// 	}
// 	else
// 	{
// 		// _handleInvalidRequest(request, fd);
// 	}
// }

// void Server::_handleGetRequest(Message &request, int fd)
// {
// }

// void Server::_handlePostRequest(Message &request, int fd)
// {
// }

// void Server::_handleDeleteRequest(Message &request, int fd)
// {
// 	// retrieve filename from request
// 	// delete file
// 	// send response
// }

// void Server::_handleInvalidRequest(Message &request, int fd)
// {
// }