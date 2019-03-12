#include <iostream>
#include <cmath>

#include "factory.hpp"

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
auto GetPluginInfo() -> PluginInfo*
{

	static PluginInfo pinfo = []{
		auto p = PluginInfo("PluginA", "0.1-alpha");
		p.registerClass<Exp>("Exp");
		p.registerClass<Log>("Log");
		return p;
	}();
	return &pinfo;
}


struct _DLLInit{
	_DLLInit(){
		std::cerr << " [TRACE] Shared library PluginA loaded OK." << std::endl;
	}
	~_DLLInit(){
		std::cerr << " [TRACE] Shared library PluginA unloaded OK." << std::endl;
	}
} dll_init;

