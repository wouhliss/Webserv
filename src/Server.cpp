/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vincentfresnais <vincentfresnais@studen    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/23 15:26:37 by wouhliss          #+#    #+#             */
/*   Updated: 2025/02/13 16:26:16 by vincentfres      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Server.hpp>

Server::Server() : _sockfd(-1)
{
}

Server::Server(const Server &server)
{
	*this = server;
}

Server::~Server()
{
	if (_sockfd > 0)
		close(_sockfd);
	for (std::map<int, int>::iterator it = fd_to_sockfd.begin(); it != fd_to_sockfd.end(); ++it)
	{
		if (it->second == _sockfd)
			close(it->first);
	}
}

Server &Server::operator=(const Server &copy)
{
	if (this == &copy)
		return (*this);
	_hostname = copy._hostname;
	_port = copy._port;
	_server_name = copy._server_name;
	_max_body_size = copy._max_body_size;
	_root = copy._root;
	_default_file = copy._default_file;
	_locations = copy._locations;
	_error_pages = copy._error_pages;
	_cgi_extensions = copy._cgi_extensions;
	_cgibin = copy._cgibin;
	_sockfd = copy._sockfd;
	return (*this);
}

int Server::getSocket(void) const
{
	return (_sockfd);
}

void Server::initSocket(void)
{
	char cwd[PATH_MAX];

	_root = std::string(getcwd(cwd, sizeof(cwd))) + "/" + _root;

	_addr.sin_family = AF_INET;
	_addr.sin_port = htons(_port);
	_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	_addr_len = sizeof(_addr);

	if ((_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		throw std::runtime_error("Error: Could not create socket");

	int opt = 1;
	if (setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		throw std::runtime_error("Error: Could not set socket options");

	if (fcntl(_sockfd, F_SETFL, fcntl(_sockfd, F_GETFL, 0) | O_NONBLOCK) < 0)
		throw std::runtime_error("Error: Could not set socket options");

	max_fd = _sockfd;
	sockfd_to_server[_sockfd] = this;
	FD_SET(_sockfd, &current_fds);

	if (bind(_sockfd, (struct sockaddr *)&_addr, _addr_len) < 0)
		throw std::runtime_error("Error: Could not bind socket");

	if (listen(_sockfd, 10) < 0)
		throw std::runtime_error("Error: Could not listen to socket");
}

std::vector<Server> Server::parseConfigFile(const std::string &filename)
{
	std::vector<Server> servers;
	std::ifstream infile(filename.c_str());
	std::string line;
	t_parser_block parser_position;

	std::cout << BLUE << "Parsing config file: " << filename << std::endl
			  << RESET;

	if (!infile.is_open())
		throw std::runtime_error("Error: could not open file");

	parser_position.server = false;
	parser_position.error = false;
	parser_position.location = false;

	while (std::getline(infile, line))
	{
		line = trim_spaces(line);

		if (line.empty() || line[0] == '#')
			continue;

		if (line == "server:")
		{
			parser_position.server = true;
			parser_position.error = false;
			parser_position.location = false;
			servers.push_back(Server());
			continue;
		}

		if (line == "error_pages:")
		{
			if (parser_position.server == false)
				throw std::runtime_error("Error: error block outside of server block");
			parser_position.location = false;
			parser_position.error = true;
			continue;
		}

		if (line == "location:")
		{
			if (parser_position.server == false)
				throw std::runtime_error("Error: location block outside of server block");
			parser_position.location = true;
			parser_position.error = false;
			servers.back().addLocation();
			
			continue;
		}

		if (parser_position.server == false)
			throw std::runtime_error("Error: block outside of server block");
		parseLine(line, servers.back(), parser_position);
	}

	infile.close();

	std::cout << BLUE << "Config file parsed successfully" << std::endl
			  << RESET;

	return (servers);
}

//Other functions

void Server::addLocation(void)
{
	_locations.push_back(Location());
}

void Server::addCgiExtension(const std::string &key, const std::string &value)
{
	if (_cgi_extensions.find(key) == _cgi_extensions.end())
		_cgi_extensions[key] = value;
}

void Server::updateErrorPage(const int error_code, const std::string &value)
{
	_error_pages[error_code] = value;
}

//Getters and Setters

void Server::setHostname(const std::string &value)
{
	_hostname = value;
}

void Server::setPort(const int value)
{
	_port = value;
}

void Server::setServerName(const std::string &value)
{
	_server_name = value;
}

void Server::setMaxBodySize(const std::size_t value)
{
	_max_body_size = value;
}

void Server::setRoot(const std::string &value)
{
	_root = value;
}

void Server::setDefaultFile(const std::string &value)
{
	_default_file = value;
}

void Server::setCgiBin(const std::string &value)
{
	_cgibin = value;
}

std::vector<Location> &Server::getLocations(void)
{
	return (_locations);
}

std::map<int, std::string> &Server::getErrorPages(void)
{
	return (_error_pages);
}

std::string &Server::getHostname(void)
{
	return (_hostname);
}

int Server::getPort(void) const
{
	return (_port);
}

std::string &Server::getServerName(void)
{
	return (_server_name);
}

std::size_t Server::getMaxBodySize(void) const
{
	return (_max_body_size);
}

std::string &Server::getRoot(void)
{
	return (_root);
}

std::string &Server::getDefaultFile(void)
{
	return (_default_file);
}

std::string &Server::getCgiBin(void)
{
	return (_cgibin);
}

std::map<std::string, std::string> &Server::getCgiExtensions(void)
{
	return (_cgi_extensions);
}
