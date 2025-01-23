/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wouhliss <wouhliss@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/23 15:16:04 by wouhliss          #+#    #+#             */
/*   Updated: 2025/01/23 17:00:51 by wouhliss         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <webserv.hpp>

int max_fd = 0;
std::map<int, Server *> sockfd_to_server;
std::map<int, int> fd_to_sockfd;
fd_set current_fds, write_fds, read_fds;

volatile sig_atomic_t loop = 1;

void siginthandle(int sig)
{
	(void)sig;
	loop = 0;
}

void loop_handle()
{
	read_fds = write_fds = current_fds;
	select(max_fd + 1, &read_fds, &write_fds, NULL, 0);
	for (int i = 0; i <= max_fd; ++i)
	{
		if (FD_ISSET(i, &read_fds))
		{
			if (sockfd_to_server.find(i) != sockfd_to_server.end())
			{
				int new_fd;
				struct sockaddr_in new_addr;
				id_t new_addrlen = sizeof(new_addr);

				new_fd = accept(i, (struct sockaddr *)&new_addr, &new_addrlen);

				FD_SET(new_fd, &current_fds);
				if (new_fd > max_fd)
					max_fd = new_fd;

				fd_to_sockfd[new_fd] = i;
			}
			else
			{
				char buffer[4096];
				int bytes_received;

				bytes_received = recv(i, buffer, sizeof(buffer), 0);
				if (bytes_received <= 0)
				{
					FD_CLR(i, &current_fds);
					if (close(i) < 0)
						std::cerr << "Error closing fd " << i << std::endl;
					fd_to_sockfd.erase(i);
					return;
				}
			}
		}
	}
}

int main(int argc, char **argv)
{
	std::string filename;

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
	if (!check_extension(filename))
	{
		std::cerr << RED << "Error: invalid file extension" << RESET << std::endl;
		return (EXIT_FAILURE);
	}
	try
	{
		std::vector<Server> servers = Server::parseConfigFile(filename);

		FD_ZERO(&current_fds);
		for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); ++it)
		{
			std::cout << it->getServerName() << '\n'
					  << it->getHostname() << '\n'
					  << it->getPort() << '\n'
					  << std::endl;

			it->initSocket();
		}
		signal(SIGINT, siginthandle);
		while (loop)
		{
			loop_handle();
		}
	}
	catch (std::exception &e)
	{
		std::cerr << RED << "Exception caught: " << e.what() << RESET << std::endl;
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}
