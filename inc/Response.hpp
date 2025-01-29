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
		void setIsDirectory(bool is_directory);
		void setHTTPVersion(const std::string &http_version);
		void setStatusCode(int status_code);
		void setStatusMessage(const std::string &status_message);

		void handleGET();
		void handlePOST();
		void handleDELETE();

		void prepareResponse();
		void defineContentType();
		void defineStatusMessage(const int status_number);
		void defineResponseHeaders();
	
	private:
		Server 		*_server;
		std::string _buffer;
		int			 _status_code;
		std::string _status_message;
		std::map<std::string, std::string> _headers;
		std::string _body;
		std::string _full_path;
		std::string _uri_attributes;
		std::string _redirection;
		std::string _content_type;
		std::string _http_version;
		bool _is_directory;
		bool _is_being_written;
		bool _is_complete;
}

#endif