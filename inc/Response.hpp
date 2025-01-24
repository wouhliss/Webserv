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
	
	private:
		std::string _status_code;
		std::string _status_message;
		std::map<std::string, std::string> _headers;
		std::string _body;
		bool _is_complete;
}

#endif