#ifndef CONFIGPARSER_HPP
# define CONFIGPARSER_HPP

# include "../inc/includes.hpp"
# include "../inc/ConfigServer.hpp"
# include "../inc/Location.hpp"
# include "../inc/Utils.hpp"

namespace configParser
{
	struct ParserBlock
	{
		bool	server;
		bool	error;
		bool	location;
	};

	//Main configuration file function, returns a ConfigServer object
	std::vector<ConfigServer>	parseConfigFile(std::string filename);
	void						parseLine(std::string line, ConfigServer &current_server, configParser::ParserBlock &parser_position);
	void						parseServerBlock(std::string key, std::string value, ConfigServer &current_server);
	void						parseLocationBlock(std::string key, std::string value, ConfigServer &current_server);
	bool						checkConfig(std::vector<ConfigServer> &servers);		
}

#endif