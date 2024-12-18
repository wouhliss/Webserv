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
		filename = "config/default.conf";
	else {
		std::cerr << "Error: too many arguments" << std::endl;
		return (EXIT_FAILURE);
	}

	//check config file extension
	if (!check_extension(filename)) {
		std::cerr << "Error: invalid file extension" << std::endl;
		return (EXIT_FAILURE);
	}

	//load and parse config file
	try {
		std::vector<ConfigServer> servers = configParser::parseConfigFile(filename);
	}
	catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}
