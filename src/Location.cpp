#include "../inc/Location.hpp"

Location::Location()
{
	_path = "";
	_redirects = "";
	_directory_listing = false;
}

Location::Location(const Location &copy)
{
	*this = copy;
}

Location::~Location()
{
}

Location &Location::operator=(const Location &copy)
{
	if (this == &copy)
		return (*this);
	_path = copy._path;
	_redirects = copy._redirects;
	_allowed_methods = copy._allowed_methods;
	_directory_listing = copy._directory_listing;
	return (*this);
}

bool Location::checkValidMethod(const char *s)
{
	const std::string methods[] = ALLOWED_METHODS;

	for (size_t i = 0; i < sizeof(methods) / sizeof(methods[0]); i++)
	{
		if (s == methods[i])
			return (true);
	}
	return (false);
}

void Location::setPath(string &s)
{
	_path = s;
}

void Location::setRedirects(string &s)
{
	_redirects = s;
}

void Location::setAllowedMethods(string &s)
{
	std::string method;
	size_t pos;

	while ((pos = s.find(" ")) != std::string::npos)
	{
		method = s.substr(0, pos);
		_allowed_methods.push_back(method);
		s.erase(0, pos + 1);
	}
	if (!checkValidMethod(s.c_str()))
		throw std::runtime_error("Error: invalid method in allowed_methods");
	_allowed_methods.push_back(s);
}

void Location::setDirectoryListing(string &s)
{
	if (s == "on")
		_directory_listing = true;
	else if (s == "off")
		_directory_listing = false;
	else
		throw std::runtime_error("Error: directory_listing's value must be set to 'on' or 'off'");
}

string &Location::getPath()
{
	return (_path);
}

string &Location::getRedirects()
{
	return (_redirects);
}

std::vector<std::string> &Location::getAllowedMethods()
{
	return (_allowed_methods);
}

bool &Location::getDirectoryListing()
{
	return (_directory_listing);
}