/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wouhliss <wouhliss@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 22:18:06 by wouhliss          #+#    #+#             */
/*   Updated: 2025/01/29 00:38:41 by wouhliss         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Client.hpp>

Client::Client() : _fd(-1), _state(0), _body_size(0), _content_length(0), _header_end(0)
{
}

Client::Client(int sock_fd) : _fd(-1), _sockfd(sock_fd), _state(0), _body_size(0), _content_length(0), _header_end(0)
{
}

Client::~Client()
{
	if (_fd > 0)
		close(_fd);
}

Client &Client::operator=(const Client &copy)
{
	if (this == &copy)
		return (*this);
	_sockfd = copy._sockfd;
	_fd = copy._fd;
	_buffer = copy._buffer;
	_state = copy._state;
	_body_size = copy._body_size;
	_content_length = copy._content_length;
	_header_end = copy._header_end;
	return (*this);
}

void Client::setFd(int fd)
{
	_fd = fd;
}

void Client::setState(int state)
{
	_state = state;
}

void Client::setMethod(const std::string &method)
{
	_method = method;
}

void Client::setPath(const std::string &path)
{
	_path = path;
}

void Client::setHttpVer(const std::string &ver)
{
	_http_ver = ver;
}

void Client::setContentLength(size_t size)
{
	_content_length = size;
}

void Client::addHeaderEnd(size_t size)
{
	_header_end += size;
}

std::map<std::string, std::string> &Client::getHeaders(void)
{
	return (_headers);
}

std::string &Client::getMethod(void)
{
	return (_method);
}

std::string &Client::getPath(void)
{
	return (_path);
}

std::string &Client::getHttpVer(void)
{
	return (_http_ver);
}

int Client::getFd(void) const
{
	return (_fd);
}

int Client::getSockFd(void) const
{
	return (_sockfd);
}

int Client::getState(void) const
{
	return (_state);
}

size_t Client::getBodySize(void) const
{
	return (_body_size);
}

size_t Client::getContentLength(void) const
{
	return (_content_length);
}

size_t Client::getHeaderEnd(void) const
{
	return (_header_end);
}

std::vector<uint8_t> &Client::getBuffer(void)
{
	return (_buffer);
}

std::vector<uint8_t> &Client::getResponseBuffer(void)
{
	return (_response_buffer);
}

void Client::appendBuffer(uint8_t *tab, size_t size)
{
	_buffer.insert(_buffer.end(), tab, tab + size);
}

void Client::appendResponseBuffer(uint8_t *tab, size_t size)
{
	_response_buffer.insert(_response_buffer.end(), tab, tab + size);
}

void Client::addHeader(const std::string &key, const std::string &value)
{
	std::cout << "Key: " << key << " Value: " << value << std::endl;
	_headers[key] = value;
	if (key == "content-length")
	{
		_content_length = std::atoll(value.c_str());
	}
}

void Client::addBodySize(size_t size)
{
	_body_size += size;
}
