#ifndef LOCATION_HPP
# define LOCATION_HPP

# include "../inc/includes.hpp"

using std::string;

# define ALLOWED_METHODS {"GET", "POST", "DELETE"}

class Location
{
	private:
		string						_path;
		string						_redirects;
		std::vector<std::string>	_allowed_methods;
		bool						_directory_listing;

	public:
		Location();
		Location(const Location& copy);
		~Location();
		Location& operator=(const Location &copy);

		void	setPath(string &s);
		void	setRedirects(string &s);
		void	setAllowedMethods(string &s);
		void	setDirectoryListing(string &s);

		string&						getPath();
		string&						getRedirects();
		std::vector<std::string>&	getAllowedMethods();
		bool&						getDirectoryListing();

		bool						checkValidMethod(std::string &s);
};

#endif