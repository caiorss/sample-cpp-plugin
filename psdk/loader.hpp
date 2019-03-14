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

#ifdef GetClassName 
  #undef GetClassName 
#endif

/** Class form managing and encapsulating shared libraries loading  */
class Plugin
{	
public:
	/** Function pointer to DLL entry-point */
	using GetPluginInfo_fp = IPluginFactory* (*) ();
	/** Name of DLL entry point that a Plugin should export */
	static constexpr const char* DLLEntryPointName = "GetPluginFactory";
	
	/** Shared library handle */
	void*        m_hnd		= nullptr;
	/** Shared library file name */
	std::string  m_file		= "";
	/** Flag to indicate whether plugin (shared library) is loaded into current process. */
	bool         m_isLoaded = false;
	/** Pointer to shared library factory class returned by the DLL
	 * entry-point function 
     */
	IPluginFactory* m_info  = nullptr;

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
			  reinterpret_cast<GetPluginInfo_fp>(dlsym(m_hnd, DLLEntryPointName));
		#else
		  auto dllEntryPoint =
			  reinterpret_cast<GetPluginInfo_fp>(GetProcAddress((HMODULE) m_hnd, DLLEntryPointName));
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

	IPluginFactory* GetInfo() const
	{
		return m_info;
	}

	void* CreateInstance(std::string const& className)
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

	IPluginFactory* addPlugin(const std::string& name)
	{	  		
		std::string fileName = name + GetExtension();
		m_plugindb[name] = Plugin(fileName);
		return m_plugindb[name].GetInfo();
	}

	IPluginFactory* GetPluginFactory(const char* pluginName)
	{
		auto it = m_plugindb.find(pluginName);
		if(it == m_plugindb.end())
			return nullptr;
		return it->second.GetInfo();
	}

	void* CreateInstance(std::string pluginName, std::string className)
	{
		auto it = m_plugindb.find(pluginName);
		if(it == m_plugindb.end())
			return nullptr;		
		return it->second.CreateInstance(className);
	}

	template<typename T>
	std::shared_ptr<T>
	CreateInstanceAs(std::string pluginName, std::string className)
	{
		void* pObj = CreateInstance(std::move(pluginName), std::move(className));
		return std::shared_ptr<T>(reinterpret_cast<T*>(pObj));
	}
	
}; /* --- End of class PluginManager --- */



#endif 
