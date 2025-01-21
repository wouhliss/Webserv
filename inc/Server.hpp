#ifndef SERVER_HPP
#define SERVER_HPP

#include "../inc/includes.hpp"
#include "../inc/ConfigServer.hpp"
#include "../inc/Location.hpp"
#include "../inc/Message.hpp"
#include "../inc/messageParser.hpp"

#define SRV_RECV_BUFFER_SIZE 4096
#define SRV_RECV_LOOP_LENGTH 255

class Server
{
private:
	// innate and network attributes
	int _socketfd;
	struct sockaddr_in _addr;
	std::string _request;
	id_t _addrlen;
	// int _global_client_id;

	// Attributes to receive from config
	size_t _port;
	std::string _server_name;
	std::string _root;
	size_t _max_body_size;
	std::string _default_file;
	std::vector<Location> _locations;
	std::map<int, std::string> _error_pages;
	std::map<int, std::string> _cookies;

	// main function to start server
	void _init_sockets();

	// Handle incoming requests
	void _handle_request(int fd);
	// Handle response messages
	// All request handling functions
	void _treatRequest(Message &request, int fd);
	void _handleGetRequest(Message &request, int fd);
	void _handlePostRequest(Message &request, int fd);
	void _handleDeleteRequest(Message &request, int fd);
	void _handleInvalidRequest(Message &request, int fd);

	// send response and access files
	void	_sendFile(int fd, const std::string &filepath);
	void	_sendError(int fd, int status_code);
	void 	_sendResponse(int fd, const std::string &body_buffer, int status_code, const std::string &type);

public:
	Server();
	Server(ConfigServer &server);
	Server(const Server &copy);
	~Server();
	Server &operator=(const Server &copy);

	void fetch();
	void handle_response(int fd);
	int get_sock_fd() const;
};

#endif