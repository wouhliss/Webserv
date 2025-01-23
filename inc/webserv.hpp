/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wouhliss <wouhliss@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/23 15:16:01 by wouhliss          #+#    #+#             */
/*   Updated: 2025/01/23 16:46:21 by wouhliss         ###   ########.fr       */
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

#define BOLD "\033[1m"
#define ITALIC "\033[3m"
#define UNDERLINE "\033[4m"

#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define RESET "\033[0m"

#define SSTR(x) static_cast<std::ostringstream &>(           \
					(std::ostringstream() << std::dec << x)) \
					.str()

bool check_extension(const std::string &str);

class Server;

extern int max_fd;
extern std::map<int, Server *> sockfd_to_server;
extern fd_set current_fds, write_fds, read_fds;

#endif