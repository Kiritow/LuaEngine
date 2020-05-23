#include "include.h"

int common_getplatform(lua_State* L)
{
	lua_pushstring(L, SDL_GetPlatform());
	return 1;
}

int common_getticks(lua_State* L)
{
	lua_pushinteger(L, SDL_GetTicks());
	return 1;
}

int common_sleep(lua_State* L)
{
	int ms = luaL_checkinteger(L, 1);
	SDL_Delay(ms);
	return 0;
}

void InitCommon(lua_State* L)
{
	lua_getglobal(L, "package");
	lua_getfield(L, -1, "loaded");
	lua_newtable(L);
	lua_setfield_function(L, "getPlatform", common_getplatform);
	lua_setfield_function(L, "getTicks", common_getticks);
	lua_setfield_function(L, "sleep", common_sleep);
	lua_setfield(L, -2, "Common");
	lua_pop(L, 2);
}
