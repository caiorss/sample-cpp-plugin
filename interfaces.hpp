#ifndef _INTERFACES_HPP_
#define _INTERFACES_HPP_

#include <string>

using pstring = std::string;

class IMathFunction
{
public:
	virtual pstring Name() const = 0;
	virtual double  Eval(double x) const = 0;	
	virtual ~IMathFunction() = default;
};

struct PluginInfo
{
	const char* name;
	const char* version;
	const char* pluginList [];
};

// class PluginManager

#endif 
