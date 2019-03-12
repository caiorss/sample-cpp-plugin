#ifndef _INTERFACES_HPP_
#define _INTERFACES_HPP_

#include <string>

using pstring = std::string;

// class PluginManager

struct IPluginInfo{
	/** Get Plugin Name */
	virtual const char* Name() const = 0 ;
	/** Get Plugin Version */
	virtual const char* Version() const = 0;
	/** Instantiate a class from its name */
	virtual void* Factory(const char* className) const = 0;
};


#endif 
