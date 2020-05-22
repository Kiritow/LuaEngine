#include "include.h"

int surface_close(lua_State* L)
{
	auto surf = lua_checkpointer<SDL_Surface>(L, 1, "LuaEngineSurface");
	SDL_FreeSurface(surf);
	return 0;
}

// shared
void put_surface(lua_State* L, SDL_Surface* surf)
{
	lua_newpointer(L, surf);
	if (luaL_newmetatable(L, "LuaEngineSurface"))
	{
		lua_setfield_function(L, "__gc", surface_close);
		lua_newtable(L);

		lua_setfield(L, -2, "__index");
	}
	lua_setmetatable(L, -2);
}
