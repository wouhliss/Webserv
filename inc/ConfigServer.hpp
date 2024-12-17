#ifndef CONFIGSERVER_HPP
# define CONFIGSERVER_HPP

# include "../inc/includes.hpp"
# include "../inc/Location.hpp"

using std::string;
using std::vector;
using std::map;

class ConfigServer
{
	private:
		//Private attributes
		string				_hostname;
		size_t				_port;
		string 				_server_name;
		size_t				_max_body_size;
		string				_root;
		string				_default_file;
		map<int, string>	_error_pages;
		map<int, string>	_cookies;
		vector<Location>	_locations;


	public:
		//Default constructors / destructors / overloads
		ConfigServer();
		ConfigServer(const ConfigServer& copy);
		~ConfigServer();
		ConfigServer& operator=(const ConfigServer &copy);

		//Getters and Setters
		void	setHostname(string &s);
		void	setPort(string &s);
		void	setServerName(string &s);
		void	setMaxBodySize(string &s);
		void	setRoot(string &s);
		void	setDefaultFile(string &s);
		void	setErrorPages(int code, string &s);
		void	setCookies(int code, string &s);

		//Location management
		void	addLocation();
		void	updateLastLocation(Location &loc);

		string&				getHostname();
		size_t&				getPort();
		string&				getServerName();
		size_t&				getSize();
		string&				getRoot();
		string&				getDefaultFile();
		vector<Location>&	getLocations();
		map<int, string>&	getErrorPages();
};

#endif