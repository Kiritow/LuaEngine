#include "include.h"

/*
class Texture
	getsize(): w: int, h: int
*/

int texture_close(lua_State* L)
{
	auto text = lua_checkpointer<SDL_Texture>(L, 1, "LuaEngineTexture");
	SDL_DestroyTexture(text);
	return 0;
}

int texture_getsize(lua_State* L)
{
	auto text = lua_checkpointer<SDL_Texture>(L, 1, "LuaEngineTexture");
	int w, h;
	if (SDL_QueryTexture(text, NULL, NULL, &w, &h) != 0)
	{
		return SDLError(L, SDL_QueryTexture);
	}
	lua_pushinteger(L, w);
	lua_pushinteger(L, h);
	return 2;
}

// shared
void put_texture(lua_State* L, SDL_Texture* text)
{
	lua_newpointer(L, text);
	if (luaL_newmetatable(L, "LuaEngineTexture"))
	{
		lua_setfield_function(L, "__gc", texture_close);
		lua_newtable(L);
		lua_setfield_function(L, "getSize", texture_getsize);
		lua_setfield(L, -2, "__index");
	}
	lua_setmetatable(L, -2);
}
