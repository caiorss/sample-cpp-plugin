
#include "interfaces.hpp"
#include <iostream>
#include <cmath>


// Forward declarations 
// class Exp; // : public IMathFunction;
// class Log; // : public IMathFunction;

// ====== Implementations of this module ==== // 

class Exp: public IMathFunction
{
public:
	Exp() = default;
	pstring Name() const {
		return "Exp";
	}
	double Eval(double x) const {
		return std::exp(x);
	}
};

class Log: public IMathFunction
{
public:
	Log() = default;
	pstring Name() const {
		return "Log";
	}
	double Eval(double x) const {
		return std::log(x);
	}
};

// ===== Factory Function - Plugin EntryPoint ==== //

extern "C"
auto factoryFunction(const char* className) -> void*
{
	auto name = std::string(className);
	if(name == "Exp")
		return new (std::nothrow) Exp;
	if(name == "Log")
		return new (std::nothrow) Log;	
	return nullptr;
}

struct _DLLInit{
	_DLLInit(){
		std::cerr << " [TRACE] Shared library PluginA loaded OK." << std::endl;
	}
	~_DLLInit(){
		std::cerr << " [TRACE] Shared library PluginA unloaded OK." << std::endl;
	}
} dll_init;

