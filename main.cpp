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
	void* m_hnd = nullptr;
	std::string m_file = "";
	bool m_isLoaded    = false;

	Plugin(){}

	explicit Plugin(std::string file)
	{
		m_file = std::move(file);
		m_hnd = ::dlopen(m_file.c_str(), RTLD_LAZY);
		m_isLoaded = true;
		assert(m_hnd != nullptr);
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
	} 
	Plugin& operator=(Plugin&& rhs)
	{
		std::swap(rhs.m_isLoaded, m_isLoaded);
		std::swap(rhs.m_hnd,  m_hnd);
		std::swap(rhs.m_file, m_file);
		return *this;
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

	void addPlugin(const std::string& name)
	{	  		
		std::string fileName = name + GetExtension();
		m_plugindb[name] = Plugin(fileName);		
	}

	void* GetObject(std::string pluginName, std::string className)
	{
		auto it = m_plugindb.find(pluginName);
		if(it == m_plugindb.end())
			return nullptr;		
		// Type signature of plugin entry-point function (factory function)
		using Function_t = void* (*) (const char* className);
		// Try get plugin factory function 
		auto factoryFunction =
			reinterpret_cast<Function_t>(it->second.GetSymbol("factoryFunction"));
		if(factoryFunction == nullptr)
			return nullptr;
		// Execute DLL entry-point 
		return factoryFunction(className.c_str());
	}

	template<typename T>
	std::shared_ptr<T>
	GetObjectAs(std::string pluginName, std::string className)
	{
		void* pObj = GetObject(std::move(pluginName), std::move(className));
		return std::shared_ptr<T>(reinterpret_cast<T*>(pObj));
	}
	
};



int main()
{
	PluginManager ma;
	ma.addPlugin("PluginA");

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
