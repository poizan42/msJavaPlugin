#pragma once

#include "plugin_api.h"

#include "interprocComm.h"

#include <sstream>
#include <string>

inline std::string dtos( double n )
{
	std::ostringstream result;
	result << n;
	return result.str();
}

inline std::string itos(int n)
{
	std::ostringstream result;
	result << n;
	return result.str();
}

class hModPluginLoader
{
public:
	inline static hModPluginLoader* get(void) {return instance;};
    inline mineserver_pointer_struct* getMineserver(void) {return mineserver;};
	hModPluginLoader(mineserver_pointer_struct* mineserver);
	~hModPluginLoader(void);
private:
	static hModPluginLoader* instance;
	mineserver_pointer_struct* mineserver;
    InterprocComm* ic;
};

