#pragma once

#ifndef __CLIENT_HPP__
#define __CLIENT_HPP__

#include <webserv.hpp>

Class Request;
Class Response;

Class Client
{
	public:
		Client();
		Client(int fd);
		Client(const Client &client);
		~Client();
		Client &operator=(const Client &copy);

		void setFd(const int fd);

		void &getFd(void);

	private:
		int	_fd;
		int _cgi_pipes[2];
		//Check if we need pointers here or not
		Request*	_request;
		Response*	_response;
}

#endif