#include "../inc/main.hpp"

bool	check_extension(std::string filename)
{
	std::string	extension = filename.substr(filename.find_last_of(".") + 1);
	std::string	extensions[] = {"conf"};

	for (size_t i = 0; i < sizeof(extensions) / sizeof(extensions[0]); i++)
	{
		if (extension == extensions[i])
			return (true);
	}
	return (false);
}

int		main(int argc, char **argv) 
{
	std::string	filename;

	//check arguments
	if (argc == 2)
		filename = argv[1];
	else if (argc == 1)
	{
		filename = "configfiles/default.conf";
		std::cout << YELLOW << "No config file specified, using default config file 'configfiles/default.conf'" << std::endl << RESET;
	}
	else {
		std::cerr << RED << "Error: too many arguments" << std::endl << RESET;
		return (EXIT_FAILURE);
	}

	//check config file extension
	if (!check_extension(filename)) {
		std::cerr << RED << "Error: invalid file extension" << std::endl << RESET;
		return (EXIT_FAILURE);
	}

	//load and parse config file
	try {
		std::vector<ConfigServer> servers = configParser::parseConfigFile(filename);
	}
	catch (std::exception &e) {
		std::cerr << RED << "Exception caught: " << e.what() << std::endl << RESET;
		return (EXIT_FAILURE);
	}

	//On affiche des infos sur les serveurs

	//On itere sur le vector de ConfigServer, on instantie chaque nouveau Server avec sa config puis on le lance

	return (EXIT_SUCCESS);
}
