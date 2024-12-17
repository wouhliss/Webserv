#ifndef MESSAGE_HPP
# define MESSAGE_HPP

# include "../inc/includes.hpp"

# define REQUEST 0
# define RESPONSE 1

using std::string;

class Message
{

	private:
		//Private attributes
		bool						_type; //0 for request, 1 for response
		string						_method;
		string						_request_target;
		string						_http_version;
		string						_status_code;
		string						_status_message;
		std::map<string, string>	_headers;
		string						_body;

	public:
		Message();
		Message(const Message& copy);
		~Message();
		Message& operator=(const Message &copy);

		//Getters and Setters
		void	setType(bool type);
		void	setMethod(string method);
		void	setRequestTarget(string request_target);
		void	setHttpVersion(string http_version);
		void	setStatusCode(string status_code);
		void	setStatusMessage(string status_message);
		void	addHeader(string key, string value);
		void	setBody(string body);

		bool&						getType();
		string&						getMethod();
		string&						getRequestTarget();
		string&						getHttpVersion();
		string&						getStatusCode();
		string&						getStatusMessage();
		std::map<string, string>&	getHeaders();
		string&						getBody();
};

#endif