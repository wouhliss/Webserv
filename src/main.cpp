#include "../inc/main.hpp"

int max_fd = 0;

fd_set currentfds, writefds, readfds;

std::map<int, std::string> request_buffer;
std::map<int, std::string> response_buffer;
std::map<int, bool> sock_fd;
std::map<int, int> fd_to_sock;
std::vector<Server *> servers;

bool check_extension(std::string filename)
{
	std::string extension = filename.substr(filename.find_last_of(".") + 1);
	std::string extensions[] = {"conf"};

	for (size_t i = 0; i < sizeof(extensions) / sizeof(extensions[0]); i++)
	{
		if (extension == extensions[i])
			return (true);
	}
	return (false);
}

void loop_handle()
{
	int select_ret;

	// Copy the current fds to the read and write fds since select is destructive
	readfds = writefds = currentfds;
	// Main select call
	select_ret = select(max_fd + 1, &readfds, &writefds, NULL, 0);
	// If select fail, we have to handle it without exiting
	if (select_ret < 0 || select_ret > FD_SETSIZE)
	{
		std::cerr << "Error in select" << std::endl;
		// loop through all fd until maxfd, and close them if they are set on read
		// ignore the _socketfd since it is the server socket
		// if they are set on write, just clear them
		for (int i = 0; i <= max_fd; i++)
		{
			if (FD_ISSET(i, &readfds) && sock_fd.find(i) == sock_fd.end())
			{
				close(i);
				FD_CLR(i, &currentfds);
			}
			else if (FD_ISSET(i, &writefds) && sock_fd.find(i) == sock_fd.end())
				FD_CLR(i, &currentfds);
		}
		return;
	}

	std::size_t pos;

	// Otherwise we handle the connexions by looping through all the fds
	for (int i = 0; i <= max_fd; i++)
	{
		// If the fd is set on read, we have an incoming connexion
		if (FD_ISSET(i, &readfds))
		{
			// If the fd is the server socket, we have to accept the connexion
			if (sock_fd.find(i) != sock_fd.end())
			{
				int new_fd;
				struct sockaddr_in new_addr;
				id_t new_addrlen = sizeof(new_addr);

				// Accept the connexion
				new_fd = accept(i, (struct sockaddr *)&new_addr, &new_addrlen);

				// Add the new fd to the current fds
				FD_SET(new_fd, &currentfds);
				// Update the maxfd if necessary
				if (new_fd > max_fd)
					max_fd = new_fd;

				fd_to_sock[new_fd] = i;
			}
			// Otherwise we have to read the incoming message
			else
			{
				char buffer[SRV_RECV_BUFFER_SIZE];
				int bytes_received;

				bytes_received = recv(i, buffer, sizeof(buffer), 0);
				if (bytes_received <= 0)
				{
					FD_CLR(i, &currentfds);
					if (close(i) < 0)
						std::cerr << "Error closing fd " << i << std::endl;
					request_buffer.erase(i);
					response_buffer.erase(i);
					fd_to_sock.erase(i);
					return;
				}
				// we check if the fd is in the map, if not we add it
				if (request_buffer.find(i) == request_buffer.end())
					request_buffer[i] = std::string(buffer, bytes_received);
				else
					request_buffer[i] += std::string(buffer, bytes_received);

				// we add conditions to break

				if (response_buffer.find(i) == response_buffer.end() && (pos = request_buffer[i].find("\r\n\r\n")) != std::string::npos)
				{
					Message message = messageParser::parseMessage(request_buffer[i]);

					std::cout << message.getMethod() << '\n'
							  << message.getBody() << '\n'
							  << message.getRequestTarget() << '\n'
							  << message.getType() << '\n'
							  << message.getHttpVersion() << '\n'
							  << std::endl;

					for (std::vector<Server *>::iterator it = servers.begin(); it != servers.end(); ++it)
					{
						if ((*it)->get_sock_fd() == fd_to_sock[i])
						{
							(*it)->handle_response(i);
						}
					}

					request_buffer[i].erase(0, pos + 4);
				}
				// add other delim check like delim the cgi here
			}
		}
		if (response_buffer.find(i) != response_buffer.end() && FD_ISSET(i, &writefds))
		{
			ssize_t bytes_sent = send(i, response_buffer[i].c_str(), response_buffer.size(), 0);
			response_buffer[i].erase(0, bytes_sent);
			if (!response_buffer[i].size())
				response_buffer.erase(i);
		}
		if (response_buffer.find(i) == response_buffer.end() && (pos = request_buffer[i].find("\r\n\r\n")) != std::string::npos)
		{
			Message message = messageParser::parseMessage(request_buffer[i]);

			std::cout << message.getMethod() << '\n'
					  << message.getBody() << '\n'
					  << message.getRequestTarget() << '\n'
					  << message.getType() << '\n'
					  << message.getHttpVersion() << '\n'
					  << std::endl;

			for (std::vector<Server *>::iterator it = servers.begin(); it != servers.end(); ++it)
			{
				if ((*it)->get_sock_fd() == fd_to_sock[i])
				{
					(*it)->handle_response(i);
				}
			}

			request_buffer[i].erase(0, pos + 4);
		}
	}
}

volatile __sig_atomic_t loop = 1;

void siginthandle(int sig)
{
	(void)sig;
	loop = 0;
}

int main(int argc, char **argv)
{
	std::string filename;

	// check arguments
	if (argc == 2)
		filename = argv[1];
	else if (argc == 1)
	{
		filename = "configfiles/default.conf";
		std::cout << YELLOW << "No config file specified, using default config file 'configfiles/default.conf'" << RESET << std::endl;
	}
	else
	{
		std::cerr << RED << "Error: too many arguments" << RESET << std::endl;
		return (EXIT_FAILURE);
	}

	// check config file extension
	if (!check_extension(filename))
	{
		std::cerr << RED << "Error: invalid file extension" << RESET << std::endl;
		return (EXIT_FAILURE);
	}

	// load and parse config file
	try
	{
		std::vector<ConfigServer> serversConfs = configParser::parseConfigFile(filename);

		FD_ZERO(&currentfds);
		for (std::vector<ConfigServer>::iterator it = serversConfs.begin(); it != serversConfs.end(); ++it)
		{
			std::cout << it->getServerName() << '\n'
					  << it->getHostaddr() << '\n'
					  << it->getPort() << '\n'
					  << std::endl;
			servers.push_back(new Server(*it));
		}
		signal(SIGINT, siginthandle);
		while (loop)
		{
			loop_handle();
		}
		for (std::vector<Server *>::iterator it = servers.begin(); it != servers.end(); ++it)
		{
			delete *it;
		}
	}
	catch (std::exception &e)
	{
		std::cerr << RED << "Exception caught: " << e.what() << RESET << std::endl;
		return (EXIT_FAILURE);
	}

	// On affiche des infos sur les serveurs

	// On itere sur le vector de ConfigServer, on instantie chaque nouveau Server avec sa config puis on le lance

	return (EXIT_SUCCESS);
}
