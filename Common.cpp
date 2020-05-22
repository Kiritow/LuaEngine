#include "include.h"

int common_getplatform(lua_State* L)
{
	lua_pushstring(L, SDL_GetPlatform());
	return 1;
}

void InitCommon(lua_State* L)
{
	lua_newtable(L);
	lua_setfield_function(L, "getPlatform", common_getplatform);
	lua_setglobal(L, "Common");
}
