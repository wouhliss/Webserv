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
void Server::fetch()
{
}

// Function to handle incoming requests

// add error handling: for CGI we have to break at the cgi delimiter if found
void Server::_handle_request(int fd)
{
	(void)fd;
}

// Function to handle response messages

void Server::handle_response(int fd)
{
	// we check if the fd is in the map, if not we return
	if (request_buffer.find(fd) == request_buffer.end())
		return;

	Message request = messageParser::parseMessage(request_buffer[fd]);
	// we check if the request is a valid request
	if (request._isValidRequest())
	{
		// then we handle the request
		_treatRequest(request, fd);

		// we write an answer based on the method invoqued
	}
}

void Server::_treatRequest(Message &request, int fd)
{
	(void)fd;
	std::string method = request.getMethod();
	if (method == "GET")
	{
		// _handleGetRequest(request, fd);
	}
	else if (method == "POST")
	{
		// _handlePostRequest(request, fd);
	}
	else if (method == "DELETE")
	{
		_handleDeleteRequest(request, fd);
	}
	else
	{
		// _handleInvalidRequest(request, fd);
	}
}

void Server::_handleGetRequest(Message &request, int fd)
{
	(void)request;
	(void)fd;
}

void Server::_handlePostRequest(Message &request, int fd)
{
	(void)request;
	(void)fd;
}

void Server::_handleDeleteRequest(Message &request, int fd)
{
	(void)fd;
	std::string filepath = _root + request.getRequestTarget();

	if (std::remove(filepath.c_str()) != 0)
	{
		std::cerr << "Error deleting file :" << filepath << std::endl;
		_sendResponse(fd, std::string(""), 500, std::string("DELETE"));
		return;
	}
	else
	{
		_sendResponse(fd, std::string(""), 200, std::string("DELETE"));
	}
}

void Server::_handleInvalidRequest(Message &request, int fd)
{
	(void)request;
	(void)fd;
}

void Server::_sendResponse(int fd, std::string body_buffer, int status_code, std::string type)
{
	(void)fd;
	(void)body_buffer;
	(void)type;
	std::string response;
	std::string response_status_line;
	std::string response_headers;
	std::string response_body;

	response_body = response;

	// status-line = HTTP-version SP status-code SP [ reason-phrase ]
	// We can ignore the reason phrase, gonna be ignored by the client anyway
	response_status_line = "HTTP/1.1 " + SSTR(status_code) + " " + "\r\n";

	// add stuff for cookies here
	// add necessary headers here

	response_body = "test";

	response_headers += "Content-Type: text/html\r\n";
	response_headers += "Content-Length: " + SSTR(response_body.size()) + "\r\n";

	response = response_status_line + response_headers + "\r\n" + response_body;

	//utiliser ca plutot

	// if (send(fd, response.c_str(), response.size(), 0) < 0)
	// {
	// 	std::cerr << "Error sending response" << std::endl;
	// 	FD_CLR(fd, &writefds);
	// 	if (FD_ISSET(fd, &readfds)) {
	// 		FD_CLR(fd, &currentfds);
	// 		FD_CLR(fd, &readfds);
	// 		if (close(fd) < 0)
	// 			std::cerr << "Error closing fd " << fd << std::endl;
	// 	}
	// }

	//c quoi cette merde
	response_buffer[fd] = response;
}

int Server::get_sock_fd(void) const
{
	return (_socketfd);
}
