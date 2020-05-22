#include "include.h"
#include <cstring>
using namespace std;

#ifndef _WIN32

int lua_errno(lua_State* L, const char* hint)
{
	char buff[1024] = { 0 };
	strerror_r(errno, buff, 1024);
	return luaL_error(L, "%s: %s", hint, buff);
}

void PlatInit()
{

}

#endif  // End of ifndef _WIN32
