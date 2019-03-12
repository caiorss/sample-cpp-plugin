#ifndef _LOADER_HPP_
#define _LOADER_HPP_

#include <cassert>
#include <map>
#include <memory>

#include "interfaces.hpp"

// Unix
#if defined(_WIN32)
  #include <windows.h>
#elif defined(__unix__)
  // APIs: dlopen, dlclose, dlopen 
  #include <dlfcn.h>
#else
  #error "Not supported operating system"
#endif 

class Plugin
{	
public:
	// Function pointer to DLL entry-point
	using GetPluginInfo_fp = IPluginInfo* (*) ();

	void*        m_hnd		= nullptr;
	std::string  m_file		= "";
	bool         m_isLoaded = false;
	IPluginInfo* m_info     = nullptr;

	Plugin()
	{
	}

	explicit Plugin(std::string file)
	{
		m_file = std::move(file);
        #if !defined(_WIN32)
		  m_hnd = ::dlopen(m_file.c_str(), RTLD_LAZY);
		#else
		  m_hnd  = (void*) ::LoadLibraryA(m_file.c_str());
        #endif 
		m_isLoaded = true;
		assert(m_hnd != nullptr);
        #if !defined(_WIN32)
		  auto dllEntryPoint =
			  reinterpret_cast<GetPluginInfo_fp>(dlsym(m_hnd, "GetPluginInfo"));
		#else
		  auto dllEntryPoint =
			  reinterpret_cast<GetPluginInfo_fp>(GetProcAddress((HMODULE) m_hnd, "GetPluginInfo"));
        #endif 
		assert(dllEntryPoint != nullptr);
		// Retrieve plugin metadata from DLL entry-point function 
		m_info = dllEntryPoint();
	}

	~Plugin()
	{
		this->Unload();
	}
	Plugin(Plugin const&) = delete;
	Plugin& operator=(const Plugin&) = delete;

	Plugin(Plugin&& rhs) 
	{
		m_isLoaded	= std::move(rhs.m_isLoaded);
		m_hnd		= std::move(rhs.m_hnd);
		m_file		= std::move(rhs.m_file);
		m_info      = std::move(rhs.m_info);
	} 
	Plugin& operator=(Plugin&& rhs)
	{
		std::swap(rhs.m_isLoaded, m_isLoaded);
		std::swap(rhs.m_hnd,  m_hnd);
		std::swap(rhs.m_file, m_file);
		std::swap(rhs.m_info, m_info);
		return *this;
	}

	IPluginInfo* GetInfo() const
	{
		return m_info;
	}

	void* GetObject(std::string const& className)
	{
		return m_info->Factory(className.c_str());
	}

	bool isLoaded() const
	{
		return m_isLoaded;
	}
	
	void Unload()
	{
		if(m_hnd != nullptr) {
            #if !defined(_WIN32)
			   ::dlclose(m_hnd);
			#else
			   ::FreeLibrary((HMODULE) m_hnd);
			#endif 
			m_hnd = nullptr;
			m_isLoaded = false;
		}
	}
};

class PluginManager
{
public:
	using PluginMap = std::map<std::string, Plugin>;
	// Plugins database 
	PluginMap m_plugindb;
	
	PluginManager()
	{								
	}
	std::string GetExtension() const 
	{
	   std::string ext;
	   #if defined (_WIN32) 	     
		 ext = ".dll"; // Windows 
	   #elif defined(__unix__) && !defined(__apple__)
		 ext = ".so";  // Linux, BDS, Solaris and so on. 
       #elif defined(__apple__)
		 ext = ".dylib"; // MacOSX 
	   #else 	 
 	     #error "Not implemented for this platform"
	   #endif 
		return ext;
	}

	Plugin& addPlugin(const std::string& name)
	{	  		
		std::string fileName = name + GetExtension();
		m_plugindb[name] = Plugin(fileName);
		return m_plugindb[name];
	}

	IPluginInfo* GetPluginInfo(const char* pluginName)
	{
		auto it = m_plugindb.find(pluginName);
		if(it == m_plugindb.end())
			return nullptr;
		return it->second.GetInfo();
	}

	void* GetObject(std::string pluginName, std::string className)
	{
		auto it = m_plugindb.find(pluginName);
		if(it == m_plugindb.end())
			return nullptr;		
		return it->second.GetObject(className);
	}

	template<typename T>
	std::shared_ptr<T>
	GetObjectAs(std::string pluginName, std::string className)
	{
		void* pObj = GetObject(std::move(pluginName), std::move(className));
		return std::shared_ptr<T>(reinterpret_cast<T*>(pObj));
	}
	
}; /* --- End of class PluginManager --- */



#endif 
