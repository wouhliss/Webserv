/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vincentfresnais <vincentfresnais@studen    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/23 15:20:32 by wouhliss          #+#    #+#             */
/*   Updated: 2025/01/24 11:16:36 by vincentfres      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <webserv.hpp>

bool check_extension(const std::string &str)
{
	if (str.find_last_of('.') == std::string::npos)
		return (false);
	return (str.substr(str.find_last_of('.')) == ".conf");
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
	else if (key == "cgi_bin")
		current_server.setCgiBin(value);
	else if (key == "cgi")
	{
		std::string cgiext = value.substr(0, value.find_first_of(" "));
		std::string cgibin = value.substr(value.find_first_of(" ") + 1);
		current_server.addCgiExtension(cgiext, cgibin);
	}
	else
		throw std::runtime_error("Error: invalid key in server block");
}

void parseLine(const std::string &line, Server &current_server, t_parser_block &parser_position)
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
