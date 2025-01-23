/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wouhliss <wouhliss@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/23 16:13:39 by wouhliss          #+#    #+#             */
/*   Updated: 2025/01/23 16:24:46 by wouhliss         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Location.hpp>

Location::Location()
{
}

Location::~Location()
{
}

void Location::addAllowedMethod(const std::string &value)
{
	if (value == "GET")
	{
		_allowed_methods |= 0b0001;
	}
	else if (value == "POST")
	{
		_allowed_methods |= 0b0010;
	}
	else if (value == "DELETE")
	{
		_allowed_methods |= 0b0100;
	}
}

void Location::setPath(const std::string &value)
{
	_path = value;
}

void Location::setRedirect(const std::string &value)
{
	_redirect = value;
}

void Location::setDirectoryListing(const bool value)
{
	_directory_listing = value;
}

uint8_t Location::getAllowedMethods(void) const
{
	return (_allowed_methods);
}

std::string &Location::getPath(void)
{
	return (_path);
}

std::string &Location::getRedirect(void)
{
	return (_redirect);
}

bool Location::getDirectoryListing(void) const
{
	return (_directory_listing);
}
