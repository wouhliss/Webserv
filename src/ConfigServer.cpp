#include "../inc/ConfigServer.hpp"

// Constructors, destructors and overloads

ConfigServer::ConfigServer()
{
	_hostaddr = "";
	_port = 0;
	_server_name = "";
	_max_body_size = 0;
	_root = "";
	_default_file = "";
}

ConfigServer::ConfigServer(const ConfigServer &copy)
{
	*this = copy;
}

ConfigServer::~ConfigServer()
{
}

ConfigServer &ConfigServer::operator=(const ConfigServer &copy)
{
	if (this == &copy)
		return (*this);
	_hostaddr = copy._hostaddr;
	_port = copy._port;
	_server_name = copy._server_name;
	_max_body_size = copy._max_body_size;
	_root = copy._root;
	_default_file = copy._default_file;
	_error_pages = copy._error_pages;
	_cookies = copy._cookies;
	_locations = copy._locations;
	return (*this);
}

// Getters and Setters

void ConfigServer::setHostaddr(string &s)
{
	_hostaddr = s;
}

void ConfigServer::setPort(string &s)
{
	_port = atoi(s.c_str());
}

void ConfigServer::setServerName(string &s)
{
	_server_name = s;
}

void ConfigServer::setMaxBodySize(string &s)
{
	_max_body_size = atoi(s.c_str());
}

void ConfigServer::setRoot(string &s)
{
	_root = s;
}

void ConfigServer::setDefaultFile(string &s)
{
	_default_file = s;
}

void ConfigServer::setErrorPages(int code, string &s)
{
	if (code < 100 || code > 599)
		throw std::runtime_error("Error: invalid error code");
	if (_error_pages.find(code) != _error_pages.end())
		throw std::runtime_error("Error: error code already exists");
	_error_pages[code] = s;
}

void ConfigServer::setCookies(int code, string &s)
{
	_cookies[code] = s;
}

void ConfigServer::addLocation()
{
	Location loc;

	_locations.push_back(loc);
}

void ConfigServer::updateLastLocation(Location &loc)
{
	_locations.back() = loc;
}

string &ConfigServer::getHostaddr()
{
	return (_hostaddr);
}

size_t &ConfigServer::getPort()
{
	return (_port);
}

string &ConfigServer::getServerName()
{
	return (_server_name);
}

size_t &ConfigServer::getSize()
{
	return (_max_body_size);
}

string &ConfigServer::getRoot()
{
	return (_root);
}

string &ConfigServer::getDefaultFile()
{
	return (_default_file);
}

vector<Location> &ConfigServer::getLocations()
{
	return (_locations);
}

map<int, string> &ConfigServer::getErrorPages()
{
	return (_error_pages);
}

// Other functions