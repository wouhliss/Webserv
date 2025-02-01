/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vincentfresnais <vincentfresnais@studen    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/23 15:16:01 by wouhliss          #+#    #+#             */
/*   Updated: 2025/02/01 16:22:07 by vincentfres      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef __WEBSERV_HPP__
#define __WEBSERV_HPP__

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <limits>
#include <vector>
#include <iterator>
#include <map>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>
#include <linux/limits.h>
#include <fcntl.h>

#include <Server.hpp>
#include <Location.hpp>
#include <Client.hpp>
#include <Request.hpp>
#include <Response.hpp>

#define BOLD "\033[1m"
#define ITALIC "\033[3m"
#define UNDERLINE "\033[4m"

#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define RESET "\033[0m"

#define BUFFER_SIZE 4096
#define MAX_CONNECTIONS 1000

#define CRLF "\r\n"
#define DOUBLECRLF "\r\n\r\n"

#define HTTP_ERROR_BAD_REQUEST 400

#define SSTR(x) static_cast<std::ostringstream &>(           \
					(std::ostringstream() << std::dec << x)) \
					.str()

inline std::string &rtrim(std::string &s)
{
	s.erase(s.find_last_not_of(" \t\n\r\f\v") + 1);
	return s;
}

inline std::string &ltrim(std::string &s)
{
	s.erase(0, s.find_first_not_of(" \t\n\r\f\v"));
	return s;
}

inline std::string &trim_spaces(std::string &s)
{
	return ltrim(rtrim(s));
}

class Server;

typedef struct ParserBlock
{
	bool server;
	bool error;
	bool location;
} t_parser_block;

bool check_extension(const std::string &str);
void parseLine(const std::string &line, Server &current_server, t_parser_block &parser_position);
void parseServerBlock(const std::string &key, const std::string &value, Server &current_server);
void parseLocationBlock(const std::string &key, const std::string &value, Server &current_server);

extern int max_fd;
extern std::map<int, Server *> sockfd_to_server;
extern std::map<int, int> fd_to_sockfd;
extern fd_set current_fds, write_fds, read_fds;

#endif