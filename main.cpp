#include <iostream>
#include "interfaces.hpp"
#include <cassert>
#include <map>
#include <memory>

// Unix
#if defined(_WIN32)
  #include <windows.h>
#elif defined(__unix__)
  // APIs: dlopen, dlclose, dlopen 
  #include <dlfcn.h>
#else
  #error "Not supported operating system"
#endif 

using pstring = std::string;


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
		m_hnd = ::dlopen(m_file.c_str(), RTLD_LAZY);
		m_isLoaded = true;
		assert(m_hnd != nullptr);

		auto dllEntryPoint = reinterpret_cast<GetPluginInfo_fp>(dlsym(m_hnd, "GetPluginInfo"));
		assert(dllEntryPoint != nullptr);
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
			::dlclose(m_hnd);
			m_hnd = nullptr;
			m_isLoaded = false;
		}
	}
	void* GetSymbol(std::string const& name)
	{
		return ::dlsym(m_hnd, name.c_str());
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



int main()
{
	PluginManager ma;
	ma.addPlugin("PluginA");
	IPluginInfo* infoA = ma.GetPluginInfo("PluginA");
	assert(infoA != nullptr);

	std::cout << " => ma.Name =  " << infoA->Name() << " - Version = " << infoA->Version() << "\n";

	// Type of pExp: std::shared_ptr<IMathFunction>
	auto pExp = ma.GetObjectAs<IMathFunction>("PluginA", "Exp");
	assert(pExp != nullptr);
	
	std::cout << "pExp->Name()    = " << pExp->Name() << "\n";
	std::cout << "pExp->Eval(3.0) = " << pExp->Eval(3.0) << "\n";
	
	auto pLog = ma.GetObjectAs<IMathFunction>("PluginA", "Log");
	std::cout << "pLog->Name()    = " << pLog->Name() << "\n";
	std::cout << "pLog->Eval(3.0) = " << pLog->Eval(3.0) << "\n";
	
	// Plugin pa{"PluginA"};
	// using EntryPoint_t = IMathFunction* (*) (const char* className);
		
	// //auto factoryFunctionA = reinterpret_cast<EntryPoint_t>(pa.GetSymbol("factoryFunction");
	// auto factoryFunctionA = pa.GetFunction<EntryPoint_t>("factoryFunction");
	// assert(factoryFunctionA != nullptr);
	

	#if 0 
	void* hnd = ::dlopen("PluginA.so", RTLD_LAZY);
	assert(hnd != nullptr);

    // Function pointer with signature of the plugin entry-point
	// function (factory function).
	using EntryPoint_t = IMathFunction* (*) (const char* className);
		
	auto factoryFunctionA = reinterpret_cast<EntryPoint_t>(::dlsym(hnd, "factoryFunction"));
	assert(factoryFunctionA != nullptr);

	IMathFunction* pExp = factoryFunctionA("Exp");
	IMathFunction* pLog = factoryFunctionA("Log");
	
	std::cout << "pExp->Name()    = " << pExp->Name() << "\n";
	std::cout << "pExp->Eval(3.0) = " << pExp->Eval(3.0) << "\n";

	std::cout << "pLog->Name()    = " << pLog->Name() << "\n";
	std::cout << "pLog->Eval(3.0) = " << pLog->Eval(3.0) << "\n";	   
	::dlclose(hnd);
	#endif 
	
	return 0;
}
