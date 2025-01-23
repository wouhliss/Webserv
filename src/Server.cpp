/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wouhliss <wouhliss@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/23 15:26:37 by wouhliss          #+#    #+#             */
/*   Updated: 2025/01/23 16:58:58 by wouhliss         ###   ########.fr       */
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
	_sockfd = copy._sockfd;
	return (*this);
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
	{
		std::cerr << "Could not create socket." << std::endl;
		return;
	}

	int opt = 1;
	if (setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		close(_sockfd);
		std::cerr << "Could not set socket options." << std::endl;
		return;
	}

	fcntl(_sockfd, F_SETFL, fcntl(_sockfd, F_GETFL, 0) | O_NONBLOCK);

	max_fd = _sockfd;
	sockfd_to_server[_sockfd] = this;
	FD_SET(_sockfd, &current_fds);

	if (bind(_sockfd, (struct sockaddr *)&_addr, _addr_len) < 0)
	{
		close(_sockfd);
		std::cerr << "Could not bind socket." << std::endl;
		return;
	}

	if (listen(_sockfd, 10) < 0)
	{
		close(_sockfd);
		std::cerr << "Could not listen to socket." << std::endl;
		return;
	}
}

typedef struct ParserBlock
{
	bool server;
	bool error;
	bool location;
} t_parser_block;

inline std::string &rtrim(std::string &s)
{
	s.erase(s.find_last_not_of(" \t\n\r\f\v") + 1);
	return s;
}

inline std::string &ltrim(std::string &s)
{
	s.erase(0, s.find_first_not_of(" \t\n\r\f\v"));
	return s;
}

inline std::string &trim_spaces(std::string &s)
{
	return ltrim(rtrim(s));
}

void parseLocationBlock(const std::string &key, const std::string &value, Server &current_server)
{
	Location loc = current_server.getLocations().back();

	if (key == "path")
		loc.setPath(value);
	else if (key == "allowed_methods")
		loc.addAllowedMethod(value);
	else if (key == "redirects")
		loc.setRedirect(value);
	else if (key == "allow_directory_listing")
		loc.setDirectoryListing(value == "on");
	else
		throw std::runtime_error("Error: invalid key in location block");
}

void parseServerBlock(const std::string &key, const std::string &value, Server &current_server)
{
	if (key == "hostaddr")
		current_server.setHostname(value);
	else if (key == "port")
		current_server.setPort(std::atoi(value.c_str()));
	else if (key == "server_name")
		current_server.setServerName(value);
	else if (key == "max_body_size")
		current_server.setMaxBodySize(std::atoll(value.c_str()));
	else if (key == "root_directory")
		current_server.setRoot(value);
	else if (key == "entry_file")
		current_server.setDefaultFile(value);
	else
		throw std::runtime_error("Error: invalid key in server block");
}

void parseLine(const std::string &line, Server &current_server, ParserBlock &parser_position)
{
	std::string key;
	std::string value;

	key = line.substr(0, line.find_first_of(":"));
	key = trim_spaces(key);
	value = line.substr(line.find_first_of(":") + 1);
	value = trim_spaces(value);

	if (value.empty())
		throw std::runtime_error("Error: unknown key");
	if (parser_position.error == true && parser_position.location == false)
		current_server.updateErrorPage(std::atoi(key.c_str()), value);
	else if (parser_position.location == true)
		parseLocationBlock(key, value, current_server);
	else
		parseServerBlock(key, value, current_server);
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

void Server::addLocation(void)
{
	_locations.push_back(Location());
}

void Server::updateErrorPage(const int error_code, const std::string &value)
{
	_error_pages[error_code] = value;
}

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
