#pragma once

#ifndef __RESPONSE_HPP__
#define __RESPONSE_HPP__

#include <webserv.hpp>

class Response
{
	public:
		Response();
		Response(const Response &response);
		~Response();
		Response &operator=(const Response &copy);

		void setRedirection(const std::string &redirection);
		void setFullPath(const std::string &full_path);
		void setURIAttributes(const std::string &uri_attributes);
		void setServer(Server *server);

		void handleGET();
		void handlePOST();
		void handleDELETE();
	
	private:
		Server 		*_server;
		std::string _status_code;
		std::string _status_message;
		std::map<std::string, std::string> _headers;
		std::string _body;
		std::string _full_path;
		std::string _uri_attributes;
		std::string _redirection;
		bool _is_complete;
}

#endif