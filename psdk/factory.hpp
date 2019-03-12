#ifndef _FACTORY_HPP_
#define _FACTORY_HPP_

#include <string> 
#include <functional>
#include <map>
#include <vector>
#include <algorithm>

#include "interfaces.hpp"

/** Macro EXPORT_CPP makes a symbol visible. */
#if defined(_WIN32)
  // MS-Windows NT 
  #define PSDK_PLUGIN_EXPORT_C extern "C" __declspec(dllexport) 
#else
  // Unix-like OSes
  #define PSDK_PLUGIN_EXPORT_C extern "C" __attribute__ ((visibility ("default")))
#endif 

class PluginInfo: public IPluginFactory
{
	// Constructor database
	using CtorItem = std::pair<std::string, std::function<void* ()>>;
	using CtorDB = std::vector<CtorItem>;
	std::string m_name;
	std::string m_version;	
	CtorDB m_ctordb;	
public:
	
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

	virtual size_t NumberOfClasses() const
	{
		return m_ctordb.size();
	}
	virtual const char* GetClassName(size_t index) const
	{
		return m_ctordb[index].first.data();
	}
	
	/** Instantiate a class from its name */
	void* Factory(const char* className) const
	{
		auto it = std::find_if(m_ctordb.begin(), m_ctordb.end(),
							   [&className](const auto& p){
								   return p.first == className;
							   });
		if(it == m_ctordb.end())
			return nullptr;
		return it->second();
	}

	template<typename AClass>
	PluginInfo& registerClass(std::string const& name)
	{
		auto constructor = []{ return new (std::nothrow) AClass; };
		m_ctordb.push_back(std::make_pair(name, constructor));
		return *this;
	}
		
};


#endif 
