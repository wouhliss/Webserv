#include "../inc/main.hpp"

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
		std::vector<Server *> servers;

		for (std::vector<ConfigServer>::iterator it = serversConfs.begin(); it != serversConfs.end(); ++it)
		{
			std::cout << it->getServerName() << '\n'
					  << it->getHostaddr() << '\n'
					  << it->getPort() << '\n'
					  << std::endl;
			servers.push_back(new Server(*it));
		}
		while (1)
		{
			// for (std::vector<Server *>::iterator it = servers.begin(); it != servers.end(); ++it)
			// {
			// 	(*(*it)).fetch();
			// }
			servers.at(0)->fetch();
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
