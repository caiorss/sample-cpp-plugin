#ifndef _FACTORY_HPP_
#define _FACTORY_HPP_

#include <string> 
#include <functional>
#include <map> 

#include "interfaces.hpp"

class PluginInfo: public IPluginInfo
{
public:
	// Constructor database  
	using CtorDB = std::map<std::string, std::function<void* ()>>;
	std::string m_name;
	std::string m_version;	
	CtorDB m_ctordb;

	PluginInfo(const char* name, const char* version):
		m_name(name),
		m_version(version)
	{
	}		
	/** Get Plugin Name */
	const char* Name() const
	{
		return m_name.data();
	}
    /** Get Plugin Version */
	const char* Version() const
	{
		return m_version.data();
	}
	template<typename T>
	PluginInfo& registerClass(std::string const& name)
	{
		m_ctordb[name] = []{ return new (std::nothrow) T; };
		return *this;
	}	
	/** Instantiate a class from its name */
	void* Factory(const char* className) const
	{
		// auto name = std::string(className);
		auto it = m_ctordb.find(className);
		if(it == m_ctordb.end()) return nullptr;
		return it->second();
	}	
};


#endif 
