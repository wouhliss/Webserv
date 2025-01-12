#include "../inc/Server.hpp"

Server::Server()
{
}

Server::Server(ConfigServer &server)
{

	_server_name = server.getServerName();
	_root = std::string(getcwd(NULL, 0)) + "/" + server.getRoot();

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

	//Init response codes here

	//SETUP addr and variables
	_global_client_id = 0;
	FD_ZERO(&_currentfds);
	FD_SET(_socketfd, &_currentfds);
	_maxfd = _socketfd;
	_timeout.tv_sec = 0;
	_timeout.tv_usec = 0;

	_init_sockets();
}

Server::Server(const Server& copy)
{
	*this = copy;
}

Server::~Server()
{
}

Server& Server::operator=(const Server &copy)
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

//Main function launched at startup, that will init all the sockets
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
void	Server::_init_sockets()
{
	//Create a new socket
	if ((_socketfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		std::cerr << "Error creating socket" << std::endl;
		exit(1);
	}

	//Set the socket options
	int opt = 1;
	if (setsockopt(_socketfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		close(_socketfd);
		std::cerr << "Error setting socket options" << std::endl;
		exit(1);
	}

	//Set the socket to non-blocking
	if (fcntl(_socketfd, F_SETFL, O_NONBLOCK) < 0)
	{
		close(_socketfd);
		std::cerr << "Error setting socket to non-blocking" << std::endl;
		exit(1);
	}

	//Bind the socket to the address and port number
	if (bind(_socketfd, (struct sockaddr *)&_addr, _addrlen) < 0)
	{
		close(_socketfd);
		std::cerr << "Error binding socket to address and port number" << std::endl;
		exit(1);
	}

	//Listen for incoming connections
	if (listen(_socketfd, 10) < 0)
	{
		close(_socketfd);
		std::cerr << "Error listening for incoming connections" << std::endl;
		exit(1);
	}
}

//main fetch function that will be inside in the main loop
//uses select to check for incoming connections and treat them accordingly
//FROM HERE we want resilience, we cannot exit on errors, we have to handle them and continue
void	Server::fetch()
{	
	int select_ret;

	//Copy the current fds to the read and write fds since select is destructive
	_readfds = _currentfds;
	//Main select call
	select_ret = select(_maxfd + 1, &_readfds, &_writefds, NULL, &_timeout);
	//If select fail, we have to handle it without exiting
	if (select_ret < 0 || select_ret > FD_SETSIZE)
	{
		std::cerr << "Error in select" << std::endl;
		//loop through all fd until maxfd, and close them if they are set on read
		//ignore the _socketfd since it is the server socket
		//if they are set on write, just clear them
		for (int i = 0; i <= _maxfd; i++)
		{
			if (FD_ISSET(i, &_readfds) && i != _socketfd)
			{
				close(i);
				FD_CLR(i, &_currentfds);
			}
			else if (FD_ISSET(i, &_writefds) && i != _socketfd)
				FD_CLR(i, &_currentfds);
		}
		return ;
	}

	//Otherwise we handle the connexions by looping through all the fds
	for (int i = 0; i <= _maxfd; i++)
	{
		//If the fd is set on read, we have an incoming connexion
		if (FD_ISSET(i, &_readfds))
		{
			//If the fd is the server socket, we have to accept the connexion
			if (i == _socketfd)
			{
				int 				new_fd;
				struct sockaddr_in 	new_addr;
				id_t 				new_addrlen = sizeof(new_addr);

				//Accept the connexion
				if ((new_fd = accept(_socketfd, (struct sockaddr *)&new_addr, &new_addrlen)) < 0 && errno != EWOULDBLOCK)
				{
					std::cerr << "Accept connexion failed for fd " << i << std::endl;
					return ;
				}

				//Set the new fd to non-blocking
				if (fcntl(new_fd, F_SETFL, O_NONBLOCK) < 0)
				{
					close(new_fd);
					std::cerr << "Error setting new incoming fd " << i << " to non-blocking" << std::endl;
					continue ;
				}

				//Add the new fd to the current fds
				FD_SET(new_fd, &_currentfds);
				//Update the maxfd if necessary
				if (new_fd > _maxfd)
					_maxfd = new_fd;
			}
			//Otherwise we have to read the incoming message
			else
			{
				_handle_request(i);
				FD_SET(i, &_writefds);
			}
		}
		//If the fd is set on write, we have to send a response
		if (FD_ISSET(i, &_writefds))
		{
			_handle_response(i);
			FD_CLR(i, &_writefds);
		}
	}
}




//Function to handle incoming requests

//add error handling: for CGI we have to break at the cgi delimiter if found
void	Server::_handle_request(int fd)
{
	char	buffer[SRV_RECV_BUFFER_SIZE];
	int		bytes_received;

	for (int i = 0 ; i < SRV_RECV_LOOP_LENGTH; i++)
	{
		bytes_received = recv(fd, buffer, SRV_RECV_BUFFER_SIZE, 0);
		if (bytes_received <= 0)
		{
			FD_CLR(fd, &_currentfds);
			FD_CLR(fd, &_writefds);
			FD_CLR(fd, &_readfds);
			if (close(fd) < 0)
				std::cerr << "Error closing fd " << fd << std::endl;
			return ;
		}
		//we check if the fd is in the map, if not we add it
		if (_request_buffer.find(fd) == _request_buffer.end())
			_request_buffer[fd] = std::string(buffer, bytes_received);
		else
			_request_buffer[fd] += std::string(buffer, bytes_received);

		//we add conditions to break
		if (_request_buffer[fd].find("\r\n\r\n") != std::string::npos)
			break ;
		//add other delim check like delim the cgi here
	}
}


//Function to handle response messages

void	Server::_handle_response(int fd)
{
	//we check if the fd is in the map, if not we return
	if (_request_buffer.find(fd) == _request_buffer.end())
		return ;
	
	Message request = messageParser::parseMessage(_request_buffer[fd]);
	//we check if the request is a valid request
	
	//then we handle the request

	//Then we write an answer based on the method invoqued
}