#include "../inc/configParser.hpp"

std::vector<ConfigServer> configParser::parseConfigFile(std::string filename)
{
	std::vector<ConfigServer> 	servers;
	std::ifstream				infile(filename);
	std::string					line;
	ConfigServer				current_server;
	configParser::ParserBlock	parser_position;

	if (!infile.is_open())
		throw std::domain_error("Error: could not open file");

	//initial position
	parser_position.server = false;
	parser_position.error = false;
	parser_position.location = false;

	while (std::getline(infile, line)) {
		//We remove the leading and trailing whitespaces
		line = Utils::trimSpaces(line);

		//We skip empty lines and comments
		if (line.empty() || line[0] == '#')
			continue;
		
		//We check the main blocks: server, error and location
		if (line == "server:") {
			parser_position.server = true;
			parser_position.error = false;
			parser_position.location = false;
			//check if this is the first server block, see if this works actually, and if the vector shouldnt contain references or pointers
			current_server = ConfigServer();
			servers.push_back(current_server);
			continue;
		}

		if (line == "error:") {
			if (parser_position.server == false)
				throw std::runtime_error("Error: error block outside of server block");
			else if (parser_position.error == true)
				throw std::runtime_error("Error: error block inside of error block");
			else if (parser_position.location == true)
				throw std::runtime_error("Error: error block inside of location block");
			parser_position.error = true;
			continue;
		}

		if (line == "location:") {
			if (parser_position.server == false)
				throw std::runtime_error("Error: location block outside of server block");
			else if (parser_position.error == true)
				throw std::runtime_error("Error: location block inside of error block");
			else if (parser_position.location == true)
				throw std::runtime_error("Error: location block inside of location block");
			parser_position.location = true;
			servers.back().addLocation(); //we create a new location for this block
			continue;
		}

		//If inside a server block, we parse the line and its values
		if (parser_position.server == false)
			throw std::runtime_error("Error: block outside of server block");
		configParser::parseLine(line, servers.back(), parser_position);
	}

	
	configParser::checkConfig(servers);
	infile.close();
	return (servers);
}

void	configParser::parseLine(std::string line, ConfigServer &current_server, configParser::ParserBlock &parser_position)
{
	std::string		key;
	std::string		value;

	//get key and value
	key = line.substr(0, line.find_first_of(":"));
	key = Utils::trimSpaces(key);
	value = line.substr(line.find_first_of(":") + 1);
	value = Utils::trimSpaces(value);

	if (value.empty())
		throw std::runtime_error("Error: no value for key");

	if (parser_position.error == true && parser_position.location == false)
		current_server.setErrorPages(std::stoi(key), value);
	else if (parser_position.location == true)
		configParser::parseLocationBlock(key, value, current_server);
	else
		configParser::parseServerBlock(key, value, current_server);
}

void	configParser::parseServerBlock(std::string key, std::string value, ConfigServer &current_server)
{
	if (key == "hostaddr")
		current_server.setHostaddr(value);
	else if (key == "port")
		current_server.setPort(value);
	else if (key == "server_name")
		current_server.setServerName(value);
	else if (key == "max_body_size")
		current_server.setMaxBodySize(value);
	else if (key == "root_directory")
		current_server.setRoot(value);
	else if (key == "entry_file")
		current_server.setDefaultFile(value);
	else
		throw std::runtime_error("Error: invalid key in server block");
}

void	configParser::parseLocationBlock(std::string key, std::string value, ConfigServer &current_server)
{
	Location	loc = current_server.getLocations().back();

	if (key == "path")
		loc.setPath(value);
	else if (key == "allowed_methods")
		loc.setAllowedMethods(value);
	else if (key == "redirects")
		loc.setRedirects(value);
	else if (key == "allow_directory_listing")
		loc.setDirectoryListing(value);
	else
		throw std::runtime_error("Error: invalid key in location block");

	current_server.updateLastLocation(loc);
}

bool	configParser::checkConfig(std::vector<ConfigServer> &servers)
{
	(void)servers;
	return (true);
}