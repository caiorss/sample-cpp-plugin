#include <iostream>
#include "loader.hpp"

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
