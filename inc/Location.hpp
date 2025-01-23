/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wouhliss <wouhliss@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/23 15:50:59 by wouhliss          #+#    #+#             */
/*   Updated: 2025/01/23 16:13:34 by wouhliss         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef __LOCATION_HPP__
#define __LOCATION_HPP__

#include <webserv.hpp>

class Location
{
public:
	Location();
	~Location();

	void addAllowedMethod(const std::string &value);

	void setPath(const std::string &value);
	void setRedirect(const std::string &value);
	void setDirectoryListing(const bool value);

	uint8_t getAllowedMethods(void) const;
	std::string &getPath(void);
	std::string &getRedirect(void);
	bool getDirectoryListing(void) const;

private:
	std::string _path;
	uint8_t _allowed_methods;
	bool _directory_listing;
	std::string _redirect;
};
#endif
