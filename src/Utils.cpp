# include "../inc/Utils.hpp"

std::string	Utils::trimSpaces(std::string s)
{
	s = s.substr(s.find_first_not_of(" \t"));
	s = s.substr(0, s.find_last_not_of(" \t") + 1);
	return (s);
}