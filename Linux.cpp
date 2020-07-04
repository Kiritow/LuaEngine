#include "include.h"
#include <cstring>
using namespace std;

#ifndef _WIN32

void put_linuxerror(lua_State* L, int errcode, const char* hint)
{
	char buff[4096] = { 0 };
	strerror_r(errcode, buff, 4096);
	if(hint)
	{
        lua_pushfstring(L, "%s: %s", hint, buff);
	}
	else
	{
        lua_pushstring(L, buff);
	}
}

void PlatInit()
{

}

#endif  // End of ifndef _WIN32
