/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wouhliss <wouhliss@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/23 15:16:04 by wouhliss          #+#    #+#             */
/*   Updated: 2025/01/29 00:57:59 by wouhliss         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <webserv.hpp>

int max_fd = 0;
std::map<int, Server *> sockfd_to_server;
std::map<int, Client *> fd_to_client;
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

				sockfd_to_server.find(i)->second->addClient(new_fd);
			}
			else
			{
				uint8_t buffer[BUFFER_SIZE + 1];
				int bytes_received;
				Client *client = fd_to_client[i];

				if (!client)
					continue;
				if (client->getResponseBuffer().size())
					continue;
				bytes_received = recv(i, buffer, sizeof(buffer), 0);
				if (bytes_received <= 0)
				{
					FD_CLR(i, &current_fds);
					sockfd_to_server[client->getSockFd()]->removeClient(i);
					continue;
				}
				client->appendBuffer(buffer, bytes_received);
				if (client->getState() == BODY)
				{
					client->addBodySize(bytes_received);
					if (client->getContentLength() <= client->getBodySize())
					{
						std::cerr << "Content-Length: " << client->getContentLength() << '\n'
								  << "Header end: " << client->getHeaderEnd() << '\n'
								  << "Body size: " << client->getBodySize() << std::endl;
						FD_CLR(i, &current_fds);
						sockfd_to_server[client->getSockFd()]->removeClient(i);
						continue;
					}
					continue;
				}
				std::string buffer_str(reinterpret_cast<char *>(client->getBuffer().data()), client->getBuffer().size());
				std::stringstream buffer_stream(buffer_str);
				std::string line;
				std::cerr << buffer_str << std::endl;
				while (std::getline(buffer_stream, line))
				{
					size_t line_size = line.size();
					line = trim(line, " \t\n\r\f\v");
					if (line.empty())
					{
						client->setState(BODY);
						client->getBuffer().erase(client->getBuffer().begin(), client->getBuffer().begin() + line_size + 1);
						client->addBodySize(client->getBuffer().size());
						break;
					}
					if (client->getState() == REQ_LINE)
					{
						std::stringstream req_line_stream(line);
						std::string tmp;
						if (req_line_stream >> tmp && isValidMethod(tmp))
						{
							client->setMethod(tmp);
						}
						if (req_line_stream >> tmp)
						{
							client->setPath(tmp);
						}
						if (req_line_stream >> tmp)
						{
							client->setHttpVer(tmp);
							client->setState(HEADERS);
						}
					}
					else if (client->getState() == HEADERS)
					{
						std::stringstream header_line_stream(line);
						std::string key;
						header_line_stream >> key;
						line.erase(line.begin(), line.begin() + key.size());
						key = key.substr(0, key.find_first_of(":"));
						key = trim(key, " \t\n\r\f\v");
						for (std::string::iterator it = key.begin(); it != key.end(); ++it)
							*it = tolower(*it);
						line = trim(line, " \t\n\r\f\v");
						client->addHeader(key, line);
					}
					client->getBuffer().erase(client->getBuffer().begin(), client->getBuffer().begin() + line_size + 1);
					client->addHeaderEnd(line_size);
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
