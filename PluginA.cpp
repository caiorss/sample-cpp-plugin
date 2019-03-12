#include <iostream>
#include <cmath>

#include <psdk/factory.hpp>

class IMathFunction
{
public:
	virtual const char* Name() const = 0;
	virtual double  Eval(double x) const = 0;	
	virtual ~IMathFunction() = default;
};


// Forward declarations 
// class Exp; // : public IMathFunction;
// class Log; // : public IMathFunction;

// ====== Implementations of this module ==== // 

class Exp: public IMathFunction
{
public:
	Exp() = default;
	const char* Name() const {
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
	const char* Name() const {
		return "Log";
	}
	double Eval(double x) const {
		return std::log(x);
	}
};

// ===== Factory Function - Plugin EntryPoint ==== //


PSDK_PLUGIN_EXPORT_C
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

