#ifndef __CLIENT_HPP__
#define __CLIENT_HPP__

#include <webserv.hpp>

class Server;
class Request;
class Response;

class Client
{
	public:
		Client();
		Client(int fd, struct sockaddr_in addr);
		Client(const Client &client);
		~Client();
		Client &operator=(const Client &copy);

		void setFd(const int fd);
		void setServer(Server *server);

		int  getFd(void);
		Request* getRequest(void);
		Response* getResponse(void);



		void readRequest();
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
};

#endif