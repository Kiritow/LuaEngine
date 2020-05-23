#include "include.h"

/*
class Window
	constructor(title: string, width: int, height: int)
	hide()
	show()
*/

int window_close(lua_State* L)
{
	auto wnd = lua_checkpointer<SDL_Window>(L, 1, "LuaEngineWindow");
	SDL_DestroyWindow(wnd);
	return 0;
}

int window_show(lua_State* L)
{
	auto wnd = lua_checkpointer<SDL_Window>(L, 1, "LuaEngineWindow");
	SDL_ShowWindow(wnd);
	return 0;
}

int window_hide(lua_State* L)
{
	auto wnd = lua_checkpointer<SDL_Window>(L, 1, "LuaEngineWindow");
	SDL_HideWindow(wnd);
	return 0;
}

int window_new(lua_State* L)
{
	const char* title = luaL_checkstring(L, 1);
	int width = luaL_checkinteger(L, 2);
	int height = luaL_checkinteger(L, 3);
	SDL_Window* wnd = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_HIDDEN);
	if (!wnd)
	{
		return SDLError(L, SDL_CreateWindow);
	}
	lua_newpointer(L, wnd);
	if (luaL_newmetatable(L, "LuaEngineWindow"))
	{
		lua_setfield_function(L, "__gc", window_close);
		lua_newtable(L);
		lua_setfield_function(L, "hide", window_hide);
		lua_setfield_function(L, "show", window_show);
		lua_setfield(L, -2, "__index");
	}
	lua_setmetatable(L, -2);
	return 1;
}

void InitWindow(lua_State* L)
{
	lua_getglobal(L, "package");
	lua_getfield(L, -1, "loaded");
	lua_pushcfunction(L, window_new);
	lua_setfield(L, -2, "Window");
	lua_pop(L, 2);
}
