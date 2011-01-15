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

class MSJavaPlugin
{
public:
	inline static MSJavaPlugin* get(void) {return instance;};
    inline mineserver_pointer_struct* getMineserver(void) {return mineserver;};
	MSJavaPlugin(mineserver_pointer_struct* mineserver);
	~MSJavaPlugin(void);
private:
	static MSJavaPlugin* instance;
	mineserver_pointer_struct* mineserver;
    InterprocComm* ic;
};

