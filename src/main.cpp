/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wouhliss <wouhliss@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/23 15:16:04 by wouhliss          #+#    #+#             */
/*   Updated: 2025/01/23 16:20:14 by wouhliss         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <webserv.hpp>

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

	// check config file extension
	if (!check_extension(filename))
	{
		std::cerr << RED << "Error: invalid file extension" << RESET << std::endl;
		return (EXIT_FAILURE);
	}

	// load and parse config file
	try
	{
		std::vector<Server> servers = Server::parseConfigFile(filename);

		// FD_ZERO(&currentfds);
		for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); ++it)
		{
			std::cout << it->getServerName() << '\n'
					  << it->getHostname() << '\n'
					  << it->getPort() << '\n'
					  << std::endl;

			// servers.back().init_sockets();
		}
		// signal(SIGINT, siginthandle);
		// while (loop)
		// {
		// 	loop_handle();
		// }
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
