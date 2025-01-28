/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wouhliss <wouhliss@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 22:18:19 by wouhliss          #+#    #+#             */
/*   Updated: 2025/01/29 00:25:41 by wouhliss         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef __CLIENT_HPP__
#define __CLIENT_HPP__

#include <webserv.hpp>

enum E_STATE
{
	REQ_LINE = 0,
	HEADERS = 1,
	BODY = 2
};

class Client
{
public:
	Client();
	Client(int sock_fd);
	~Client();
	Client &operator=(const Client &copy);

	int getFd(void) const;
	int getSockFd(void) const;
	int getState(void) const;
	size_t getBodySize(void) const;
	size_t getContentLength(void) const;
	size_t getHeaderEnd(void) const;
	std::string &getMethod(void);
	std::string &getPath(void);
	std::string &getHttpVer(void);
	std::map<std::string, std::string> &getHeaders(void);

	std::vector<uint8_t> &getBuffer(void);
	std::vector<uint8_t> &getResponseBuffer(void);

	void appendBuffer(uint8_t *tab, size_t size);
	void appendResponseBuffer(uint8_t *tab, size_t size);

	void addHeader(const std::string &key, const std::string &value);
	void addBodySize(size_t size);
	void addHeaderEnd(size_t size);

	void setFd(int fd);
	void setState(int state);
	void setMethod(const std::string &method);
	void setPath(const std::string &path);
	void setHttpVer(const std::string &ver);
	void setContentLength(size_t size);

private:
	int _fd;
	int _sockfd;
	int _state;
	size_t _body_size;
	size_t _content_length;
	size_t _header_end;
	std::vector<u_int8_t> _buffer;
	std::vector<u_int8_t> _response_buffer;
	std::string _method;
	std::string _path;
	std::string _http_ver;
	std::map<std::string, std::string> _headers;
};
#endif
