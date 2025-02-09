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
		Client(int fd, struct sockaddr_in addr);
		Client(const Client &client);
		~Client();
		Client &operator=(const Client &copy);

		void setFd(const int fd);
		void &getFd(void);

		void readRequest(std::string &buffer);
		void processRequest();
		void sendResponse();

	private:
		int	_fd;
		int _cgi_pipes[2];
		struct sockaddr_in _addr;
		//Check if we need pointers here or not
		Request*	_request;
		Response*	_response;
		Server*		_server;
}

#endif