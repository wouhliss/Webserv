#ifndef MESSAGEPARSER_HPP
# define MESSAGEPARSER_HPP

# include "../inc/includes.hpp"
# include "../inc/utils.hpp"
# include "../inc/Message.hpp"

namespace messageParser
{
	Message		parseMessage(std::string flux);
	void		parseRequestLine(std::string line, Message &message);
}

#endif