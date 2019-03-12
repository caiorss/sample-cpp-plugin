#include <iostream>
#include <psdk/loader.hpp> 

class IMathFunction
{
public:
	virtual const char* Name() const = 0;
	virtual double  Eval(double x) const = 0;	
	virtual ~IMathFunction() = default;
};


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
	
	return 0;
}
